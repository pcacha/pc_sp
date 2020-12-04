/*

    Generování Konečného Automatu

    Modul hash_table.c
    Modul poskytuje funkce pro ovládání hashovací tabulky.
    Tabulka přijímá dvojici klíč a hodnota, kde kíčem je textový řetězec a
    hodnoutou je celé číslo typu int. Přidávání prvků i jejich vyhledávání
    probíha v konstantním čase.


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

    Funkce nastaví vlastnosti předané tabulky na jejich výchozí hodnoty.
   ____________________________________________________________________________
*/
void initialize_hash_table(hash_table *table) {
  if(table == NULL) {
    return;
  }

  table->items_count = 0;
  table->array_lenght = ARRAY_LENGHT;
  table->values = (table_item **) malloc(ARRAY_LENGHT * sizeof(table_item *));

  if(table->values == NULL) {  /* pokud se alokace paměti nepodařila, ukončíme program */
    terminate(RUNTIME_ERR_MSG);
  }
  memset((void *) table->values, 0, ARRAY_LENGHT * sizeof(table_item *));  /* vynulujeme nově získané pole */

  increase_block_count();  /* zvětšíme počet alokovaných bloků paměti */
}

/* ____________________________________________________________________________

    static void expand_hash_table(hash_table **table)

    Zvětší počet zřetezených seznamů hashovací tabulky na dvojnásobek za účelem uchování konstatních časů.
    Tabulka předaná v prarametru bude přetvořena.
   ____________________________________________________________________________
*/
static void expand_hash_table(hash_table **table) {
  int new_array_lenght, i;
  hash_table *new_table;
  table_item *current;

  if(table == NULL || *table == NULL) {  /* ošetření vstupu */
    terminate(RUNTIME_ERR_MSG);
  }

  new_array_lenght = ((*table)->array_lenght) * 2;  /* velikost zvětšeného pole */

  new_table = malloc(sizeof(hash_table));  /* nová hashovací tabulka k překpírování prvků */
  if(new_table == NULL) {  /* pokud se nepodařila alokace paměti */
    free_hash_table(*table);
    terminate(RUNTIME_ERR_MSG);
  }
  increase_block_count();

  new_table->items_count = 0;
  new_table->array_lenght = new_array_lenght;
  new_table->values = (table_item **) malloc(new_array_lenght * sizeof(table_item *));  /* inicializace tabulky s dvakrát větším polem */

  if(new_table->values == NULL) {
    free_hash_table(new_table);  /* při nezdařené alokaci vrátíme paměť do původního stavu a ukončíme program */
    free_hash_table(*table);
    terminate(RUNTIME_ERR_MSG);
  }
  memset((void *) new_table->values, 0, new_array_lenght * sizeof(table_item *));
  increase_block_count();

  for(i = 0; i < (*table)->array_lenght; i++) { /* zde dojde v cyklu ke vložení všech prvků ze staré tabulky do nové tabulky, tím se jim nastaví správně nové indexy podle hashovacího kódu */
    current = (*table)->values[i];
    while(current != NULL) {
      add_to_table(&new_table, current->key, current->value);
      current = current->next;
    }
  }

  free_hash_table(*table);  /* uvolníme starou tabulku */

  *table = new_table;  /* nastavíme ukazatel na novou tabulku */
}

/* ____________________________________________________________________________

    static int get_hash_code(char key[MAX_VERTEX_NAME_LEN], hash_table *table)

    Získá hashovací kód pro předaný textový řetězec.
   ____________________________________________________________________________
*/
static int get_hash_code(char key[MAX_VERTEX_NAME_LEN], hash_table *table) {
  int i, code = 0;

  if(key == NULL) {
    free_hash_table(table);
    terminate(RUNTIME_ERR_MSG);
  }

  for(i = 0; i < (int) strlen(key); i++) {  /* hashovací kód je suma součinu indexu znaku a jeho číselné reprezentace */
    code += (i + 1) * key[i];
  }

  return code;
}

/* ____________________________________________________________________________

    void add_to_table(hash_table **table_ptr_ptr, char key[MAX_VERTEX_NAME_LEN], int value)

    Funkce přidá nový pár (klíč a hodnota) do tabulky.
    Index prvku získá pomocí hashovacího kódu a v případě překročení limitu zvětší tabulku.
   ____________________________________________________________________________
*/
void add_to_table(hash_table **table_ptr_ptr, char key[MAX_VERTEX_NAME_LEN], int value) {
  int index;
  table_item *new_item;
  hash_table *table;

  if(table_ptr_ptr == NULL || *table_ptr_ptr == NULL) { /* ošetření vstupu */
    terminate(RUNTIME_ERR_MSG);
  }
  if(key == NULL) {
    free_hash_table(*table_ptr_ptr);
    terminate(RUNTIME_ERR_MSG);
  }

  table = *table_ptr_ptr;

  index = get_hash_code(key, table) % table->array_lenght;  /* získání indexu pro vložení nové hodnoty */

  new_item = (table_item *) malloc(sizeof(table_item));  /* vyhradíme místo pro nový prvek */
  if(new_item == NULL) {  /* při neúspěšné alokaci paměti ukončíme program */
    free_hash_table(table);
    terminate(RUNTIME_ERR_MSG);
  }
  increase_block_count();  /* zvýšení početu obsazených bloků paměti */

  if(table->values[index] != NULL) {  /* pokud na indexu již existuje prvek */
    strcpy(new_item->key, (table->values[index])->key);  /* vložíme nový prvek na první místo spojoého seznamu odsunutím původně prvního prvku na druhé místo */
    new_item->value = (table->values[index])->value;
    new_item->next = (table->values[index])->next;

    strcpy((table->values[index])->key, key);
    (table->values[index])->value = value;
    (table->values[index])->next = new_item;

  }
  else {
    strcpy(new_item->key, key);  /* pokud je prvek první stačí ho vložit */
    new_item->value = value;
    new_item->next = NULL;
    table->values[index] = new_item;
  }

  table->items_count++;

  if((table->items_count / (double) (table->array_lenght)) > MAX_AVG_ITEM_COUNT) {  /* zvětšíme pole pokud překročíme maximální průměrný počet prvků */
    expand_hash_table(table_ptr_ptr);
  }
}

/* ____________________________________________________________________________

    static void free_items_linked_list(table_item *item)

    Rekurzivní funkce pro uvolňování prvků zřetězeného seznamu.
   ____________________________________________________________________________
*/
static void free_items_linked_list(table_item *item) {
  if(item == NULL) {
    return;
  }

  if(item->next != NULL) {  /* pokud má prvek potomka, musíme ho nejprve uvlonit */
    free_items_linked_list(item->next);
  }

  free(item);  /* uvolníme a snížíme počet alokovaných bloků paměti */
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

  free(table->values);  /* uvolníme plole seznamů */
  decrease_block_count();

  free(table);  /* následně uvolníme celou tabulku */
  decrease_block_count();
}

/* ____________________________________________________________________________

    int get_value(hash_table *table, char key[MAX_VERTEX_NAME_LEN])

    Podle zadaného klíče najde příslušnou hodnotu z předané hashovací tabulky.
   ____________________________________________________________________________
*/
int get_value(hash_table *table, char key[MAX_VERTEX_NAME_LEN]) {
  int index;
  table_item *current;

  if(table == NULL || key == NULL) {
    free_hash_table(table);
    terminate(RUNTIME_ERR_MSG);
  }

  index = get_hash_code(key, table) % table->array_lenght;  /* získání indexu, kde by se prvek s daným klíčem měl nacházet */

  current = table->values[index];  /* procházíme seznam dokud nenalazneme prvek s hledaným klíčem */
  while(current != NULL) {
    if(strcmp(current->key, key) == 0) {  /* daný klíč nalezen */
      return current->value;
    }
    current = current->next;
  }

  free_hash_table(table);
  terminate(RUNTIME_ERR_MSG);
  return FOUND_NOTHING;
}
