/*

    Generování Konečného Automatu

    Modul hash_table.c
    Modul poskytuje funkce pro ovládání hashovací tabulky zaměřené na integerovské hodnoty.
    Položky se do tabulky nepřidávají v páru klíč a hodnota, ale pouze hodnota.
    Duplicitní hodnoty nemohou být přidány.


    Dialekt : ANSI C
    Překladač: jakýkoliv ANSI C-kompatibilní

*/

#include <stdlib.h>
#include <string.h>
#include "defs.h"
#include "hash_table.h"
#include "memory_observer.h"
#include "err_manager.h"

/* ____________________________________________________________________________

    void initialize_hash_table(hash_table *table)

    Funkce zinicializje proměnné předané tablulky na výchozí hodnoty.
    Vrací true při úspěchu, false při neúspěchu.
   ____________________________________________________________________________
*/
void initialize_hash_table(hash_table *table) {
  if(table == NULL) {
    return;
  }

  table->items_count = 0;
  table->array_lenght = ARRAY_LENGHT;
  table->values = (table_item **) malloc(ARRAY_LENGHT * sizeof(table_item *));

  if(table->values == NULL) {
    terminate(RUNTIME_ERR_MSG);
  }
  memset((void *) table->values, 0, ARRAY_LENGHT * sizeof(table_item *));

  increase_block_count();  /* zvětšíme počet alokovaných bloků paměti */
}

/* ____________________________________________________________________________

    static int expand_int_set(int_set **set)

    Zvětší počet zřetezených seznamů na dvojnásobek za účelem uchování konstatních časů.
    Tabulka předaná v prarametru bude přetvořena.
   ____________________________________________________________________________
*/
static void expand_hash_table(hash_table **table) {
  int new_array_lenght, i;
  hash_table *new_table;
  table_item *current;

  new_array_lenght = ((*table)->array_lenght) * 2;  /* velikost zvětšeného pole */

  new_table = malloc(sizeof(hash_table));  /* nová hashovací tabulka k překpírování prvků */
  if(new_table == NULL) {
    free_hash_table(*table);
    terminate(RUNTIME_ERR_MSG);
  }
  increase_block_count();

  new_table->items_count = 0;
  new_table->array_lenght = new_array_lenght;
  new_table->values = (table_item **) malloc(new_array_lenght * sizeof(table_item *));  /* inicializace tabulky s dvakrát větším polem */

  if(new_table->values == NULL) {
    free_hash_table(new_table);  /* při nezdařené alokaci vrátíme paměť do původního stavu a vrátíme false */
    free_hash_table(*table);
    terminate(RUNTIME_ERR_MSG);
  }
  memset((void *) new_table->values, 0, new_array_lenght * sizeof(table_item *));
  increase_block_count();

  for(i = 0; i < (*table)->array_lenght; i++) { /* zde dojde v cyklu k překopírování všech prvků do nové tabulky */
    current = (*table)->values[i];
    while(current != NULL) {
      add_to_table(&new_table, current->key, current->value);
      current = current->next;
    }
  }

  free_hash_table(*table);  /* uvolníme starou tabulku */

  *table = new_table;
}

/* ____________________________________________________________________________

    int get_hash_code(char key[MAX_VERTEX_NAME_LEN], hash_table *table)

    <popis>
   ____________________________________________________________________________
*/
int get_hash_code(char key[MAX_VERTEX_NAME_LEN], hash_table *table) {
  int i, code = 0;

  if(key == NULL) {
    free_hash_table(table);
    terminate(RUNTIME_ERR_MSG);
  }

  for(i = 0; i < (int) strlen(key); i++) {
    code += (i + 1) * key[i];
  }

  return code;
}

/* ____________________________________________________________________________

    int add_int_to_set(int_set **set_ptr, int value)

    Funkce se pokusí přidat novou integerovou hondotu do tabulky.
    Vrací specifické číselné kódy při úspěchu, neúspěchu (prvek je již vložen) a chybě.
   ____________________________________________________________________________
*/
void add_to_table(hash_table **table_ptr, char key[MAX_VERTEX_NAME_LEN], int value) {
  int index;
  table_item *new_item;
  hash_table *table;

  table = *table_ptr;

  index = get_hash_code(key, table) % table->array_lenght;  /* získání indexu nové hodnoty */

  new_item = (table_item *) malloc(sizeof(table_item));  /* vyhradíme místo pro nový prvek */
  if(new_item == NULL) {  /* při neúspěšné alokaci paměti vrátíme kód chyby */
    free_hash_table(table);
    terminate(RUNTIME_ERR_MSG);
  }
  increase_block_count();  /* zvýšení početu obsazených bloků */

  if(table->values[index] != NULL) {
    strcpy(new_item->key, (table->values[index])->key);
    new_item->value = (table->values[index])->value;
    new_item->next = (table->values[index])->next;

    strcpy((table->values[index])->key, key);
    (table->values[index])->value = value;
    (table->values[index])->next = new_item;  /* vložíme novou hodnotu na první místo odsunutím původně prvního prvku na druhé místo zřetězeného seznamu */

  }
  else {
    strcpy(new_item->key, key);
    new_item->value = value;
    new_item->next = NULL;
    table->values[index] = new_item;  /* pokud je prvek první stačí ho vložit */
  }

  table->items_count++;

  if((table->items_count / (double) (table->array_lenght)) > MAX_AVG_ITEM_COUNT) {  /* zvětšíme pole pokud překročíme maximální průměrný počet prvků */
    expand_hash_table(table_ptr);
  }
}

/* ____________________________________________________________________________

    static void free_int_items_linked_list(int_item *item)

    Rekurzivní funkce pro uvolňování prvků zřetězeného seznamu.
   ____________________________________________________________________________
*/
static void free_items_linked_list(table_item *item) {
  if(item->next != NULL) { /* pokud má prvek potomka, musíme ho nejprve odstranit */
    free_items_linked_list(item->next);
  }

  free(item);
  decrease_block_count();
}

/* ____________________________________________________________________________

    void free_hash_table(hash_table *table)

    Uvolní z paměti předanou tabulku.
   ____________________________________________________________________________
*/
void free_hash_table(hash_table *table) {
  int i;
  table_item *current;

  if(table == NULL) {
    return;
  }

  for(i = 0; i < table->array_lenght; i++) { /* pomocí rekurzivní funkce v cyklu uvlolníme všechny prvky zřetězených seznamů */
    current = table->values[i];
    if(current != NULL) {
      free_items_linked_list(current);
    }
  }

  free(table->values);
  decrease_block_count();

  free(table);
  decrease_block_count();
}

/* ____________________________________________________________________________

    <popis>

    <popis>
   ____________________________________________________________________________
*/
int get_value(hash_table *table, char key[MAX_VERTEX_NAME_LEN]) {
  int index;
  table_item *current;

  if(table == NULL || key == NULL) {
    free_hash_table(table);
    terminate(RUNTIME_ERR_MSG);
  }

  index = get_hash_code(key, table) % table->array_lenght;  /* získání indexu nové hodnoty */

  current = table->values[index];
  while(current != NULL) {
    if(strcmp(current->key, key) == 0) {
      return current->value;
    }
    current = current->next;
  }

  free_hash_table(table);
  terminate(RUNTIME_ERR_MSG);
  return FOUND_NOTHING;
}
