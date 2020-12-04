/*

    Generování Konečného Automatu

    Hlavičkový soubor hash_table.h
    Detailní informace viz hash_table.c

    Dialekt : ANSI C
    Překladač: jakýkoliv ANSI C-kompatibilní

*/

#ifndef _HASH_TABLE_H
#define _HASH_TABLE_H

#include "loader.h"

/* ____________________________________________________________________________

    Konstanty
   ____________________________________________________________________________
*/
#define MAX_AVG_ITEM_COUNT 20  /* maximální průměrný počet položek v jednom zřetězeném seznamu tabulky */
#define ARRAY_LENGHT 10  /* startovní velikost pole hashovací tabulky */
#define FOUND_NOTHING -1 /* konstanta značí, že nebylo nic nalezeno */

/* ____________________________________________________________________________

    Struktury
   ____________________________________________________________________________
*/

/* ____________________________________________________________________________

    Struktura definující jednu položku hashovací tabulky uchovávající klíč typu textový řetězec a celočíselnou hodnotu typu int.
    Tyto pložky tvoří zřetězené seznamy.
   ____________________________________________________________________________
*/
typedef struct the_table_item {
    char key[MAX_VERTEX_NAME_LEN];  /* klíč */
    int value;  /* uchovávaná hodnota */
    struct the_table_item *next;  /* odkaz na další prvek stejného typu */
} table_item;

/* ____________________________________________________________________________

    Strukura představuje hashovací tabulku umožňující přidávat páry (klíč, hodnota) v konstantním čase a také
    hledat hodnotu na základě klíče v konstatním čase.
   ____________________________________________________________________________
*/
typedef struct the_hash_table {
    int items_count;  /* celkový počet položek v tabulce */
    int array_lenght;  /* délka pole zřetězených seznamů */
    table_item **values;  /* pole zřetězených seznamů */
} hash_table;

/* ____________________________________________________________________________

    Prototypy Funkcí
   ____________________________________________________________________________
*/
void initialize_hash_table(hash_table *table);
void add_to_table(hash_table **table_ptr_ptr, char key[MAX_VERTEX_NAME_LEN], int value);
void free_hash_table(hash_table *table);
int get_value(hash_table *table, char key[MAX_VERTEX_NAME_LEN]);

#endif
