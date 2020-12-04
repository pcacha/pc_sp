/*

    Generování Konečného Automatu

    Modul loader.c
    Modul se specializuje na načtení definice automatu z textového souboru.
    Automat musí být popsán jazykem DOT (pro nástroj Graphviz).


    Dialekt : ANSI C
    Překladač: jakýkoliv ANSI C-kompatibilní

*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "loader.h"
#include "err_manager.h"
#include "defs.h"
#include "memory_observer.h"
#include "hash_table.h"

/* ____________________________________________________________________________

    Globální Proměnné Modulu
   ____________________________________________________________________________
*/
hash_table *table = NULL;  /* hashovací tabulka prvků s klíčem typu string a hodnotou typu int */

/* ____________________________________________________________________________

    static void free_table_and_terminate()

    Uvolní hashovací tabulku a ukončí program s chybovým výpisem.
   ____________________________________________________________________________
*/
static void free_table_and_terminate() {
  free_hash_table(table);
  terminate(RUNTIME_ERR_MSG);
}

/* ____________________________________________________________________________

    static char *left_trim(char *line)

    Ořízne prázdné znaky, kterými začíná textový řetězec.
   ____________________________________________________________________________
*/
static char *left_trim(char *line) {
  if(line == NULL) {
    return NULL;
  }

  while(isspace(*line)) {  /* pokud znak není viditelný, posuneme ukazatel na řetězec */
    line++;
  }
  return line;
}

/* ____________________________________________________________________________

    static void parse_vertex(int *vertex_label, char *vertex_name, char *line)

    Funkce získá z textového řetězce jméno vrcholu a jeho typ.
    Typy jsou normální, vstupní a výstupní vrchol.
   ____________________________________________________________________________
*/
static void parse_vertex(int *vertex_label, char *vertex_name, char *line) {
  int i;

  /* ošetření vstupu */
  if(vertex_label == NULL || vertex_name == NULL || line == NULL) {
    free_table_and_terminate();
  }
  memset((void *) vertex_name, '\0', MAX_VERTEX_NAME_LEN);

  for(i = 0; i < (int) strlen(line); i++) {  /* iterujeme přes všechny znaky řetězce */
    if(line[i] == ';') {  /* konec řádky -> řetězec obsahuje pouze název vrcholu */
      line[i] = '\0';
      strcpy(vertex_name, line);  /* uložíme název vrcholu */
      *vertex_label = ORDINARY_VERTEX;  /* vrchol není ani vstupní ani výstupní */
      return;
    }

    if(line[i] == '[') {  /* pokud nalezneme hranatou závorku, budou následovat parametry */
      line[i] = '\0';
      strcpy(vertex_name, line);  /* získáme název */

      if(line[i + LABEL_DIST] == 's') {  /* pokud po posunutí pointeru o délku popisku získáme 's', vrchol je vstupní, pokud ne musí být výstupní */
        *vertex_label = INPUT_VERTEX;
      }
      else {
        *vertex_label = OUTPUT_VERTEX;
      }
      return;
    }
  }
}

/* ____________________________________________________________________________

    static char *get_and_trim_line(char *line, FILE *source_file)

    Funkce načte novou řádku ze soubouru a ořížne neviditelné znaky zleva.
   ____________________________________________________________________________
*/
static char *get_and_trim_line(char *line, FILE *source_file) {
  if(source_file == NULL || line == NULL) {
    free_table_and_terminate();
  }

  if(fgets(line, LINE_LEN, source_file) == NULL) {  /* načtení nové řádky */
    free_table_and_terminate();
  }
  return left_trim(line);  /* ořízneme neviditelné znaky */
}

/* ____________________________________________________________________________

    static void skip_header(char line[], FILE *source_file)

    Přeskočí hlavičku soubour nenesoucí žádné relevantní informace.
   ____________________________________________________________________________
*/
static void skip_header(char line[], FILE *source_file) {
  const int HEADER_LINES_COUNT = 3;
  int i;

  if(source_file == NULL || line == NULL) {
    free_table_and_terminate();
  }

  for(i = 0; i < HEADER_LINES_COUNT; i++) {  /* přeskočení hlavičky souboru, složené závorky a komentáře */
    if(fgets(line, LINE_LEN, source_file) == NULL) {
      free_table_and_terminate();
    }
  }
}

/* ____________________________________________________________________________

    static void expand_array(int *vertices_arr_size, int **vertices)

    Zvětší pole integerů na dvojnásobek.
   ____________________________________________________________________________
*/
static void expand_array(int *vertices_arr_size, int **vertices) {
  int *new_arr;

  if(vertices_arr_size == NULL || vertices == NULL || *vertices == NULL) {
    free_table_and_terminate();
  }

  *vertices_arr_size *= 2;
  new_arr = (int *) realloc(*vertices, *vertices_arr_size * sizeof(int));  /* pokus o rozšíření pole */

  if(new_arr == NULL) {  /* rozšíření se nezdařilo */
    free_table_and_terminate();
  }

  *vertices = new_arr;
}

/* ____________________________________________________________________________

    static void parse_edge(char start_vertex_name[MAX_VERTEX_NAME_LEN], char end_vertex_name[MAX_VERTEX_NAME_LEN], char *edge_char, char *line)

    Funkce získá z textového řetězce informace o hraně. Jméno výstupního vrcholu, vstupního vrcholu a znak aktivující přechod po hraně.
   ____________________________________________________________________________
*/
static void parse_edge(char start_vertex_name[MAX_VERTEX_NAME_LEN], char end_vertex_name[MAX_VERTEX_NAME_LEN], char *edge_char, char *line) {
  int i;

  if(start_vertex_name == NULL || end_vertex_name == NULL || edge_char == NULL || line == NULL) {
    free_table_and_terminate();
  }

  for(i = 0; i < (int) strlen(line); i++) {  /* iterujeme přes všechny znaky řetězce */
    if(line[i] == '-' && line[i + 1] == '>') {  /* pokud jsme nalezli předěl mezi počátečním a koncovým vrcholem hrany ('->') */
      line[i] = '\0';  /* nastavíme konec řetězce na správně místo a zkopírujeme název počátečního vrcholu */
      strcpy(start_vertex_name, line);
      line = &line[i + 2];  /* posuneme ukazatel na začátek názvu koncového vrcholu */
      break;
    }
  }

  for(i = 0; i < (int) strlen(line); i++) {
    if(line[i] == '[') {  /* nalezneme znak, který již nenáleží názvu */
      line[i] = '\0';  /* zkopírujeme název koncového vrcholu */
      strcpy(end_vertex_name, line);
      *edge_char = line[i + LABEL_DIST];  /* znak označující aktivaci přechodu po hraně je ve odsazen o délku popisku */
      return;
    }
  }
}

/* ____________________________________________________________________________

    static void load_edges(FILE **source_file_ptr, int vertex_count, edge ***edges)

    Funkce načte všechny hrany definované v souboru. Uloží je do zřetězených seznamů
   ____________________________________________________________________________
*/
static void load_edges(FILE **source_file_ptr, int vertex_count, edge ***edges) {
  char line[LINE_LEN], start_vertex_name[MAX_VERTEX_NAME_LEN], end_vertex_name[MAX_VERTEX_NAME_LEN], *working_line, edge_char;
  int start_vertex_index, end_vertex_index;
  FILE *source_file;
  edge *new_edge;

  if(source_file_ptr == NULL || *source_file_ptr == NULL || vertex_count == 0 || edges == NULL) {
    free_table_and_terminate();
  }

  source_file = *source_file_ptr;

  *edges = (edge **) malloc(vertex_count * sizeof(edge *));  /* pokus o alokaci paměti pro reprezentaci hran */

  if(*edges == NULL) {
    free_table_and_terminate();
  }
  memset((void *) *edges, 0, vertex_count * sizeof(edge *));  /* pole vynulujeme a zvášíme počet alokovaných bloků paměti */
  increase_block_count();

  working_line = get_and_trim_line(line, source_file);  /* načteme další řádku souboru */

  while(working_line[0] != FILE_END_CHAR) {  /* dokud nenarazíme na komentář označující konec výčtu hran */
    parse_edge(start_vertex_name, end_vertex_name, &edge_char, working_line);  /* získáme informace o hraně */

    start_vertex_index = get_value(table, start_vertex_name);  /* získáme index vrcholů v poli vrcholů podle jejich jmen pomocí hashovací tabulky */
    end_vertex_index = get_value(table, end_vertex_name);

    new_edge = (edge *) malloc(sizeof(edge));  /* alokujeme paměť pro novou hranu, zareagujeme na neúspěch a případně zvýšíme počet alokovaných bloků*/
    if(new_edge == NULL) {
      free_table_and_terminate();
    }
    increase_block_count();

    new_edge->dest_index = end_vertex_index;  /* nastavíme hodnoty pro novou hranu */
    new_edge->edge_char = edge_char;
    new_edge->next = NULL;

    if((*edges)[start_vertex_index] != NULL) {  /* pokud již existuje pčátení prvek zřetězeného seznamu hran tak ho posuneme na druhé místo */
      new_edge->next = (*edges)[start_vertex_index];
    }
    (*edges)[start_vertex_index] = new_edge;

    working_line = get_and_trim_line(line, source_file);  /* načteme novou řádku */
  }
}

/* ____________________________________________________________________________

    void load_graph(FILE **source_file_ptr, int *vertex_count, int *starting_vertex, int **vertices, edge ***edges)

    Funkce načte z textového souboru postupně vrcholy a hrany grafu. Soubor následně uzavře.
   ____________________________________________________________________________
*/
void load_graph(FILE **source_file_ptr, int *vertex_count, int *starting_vertex, int **vertices, edge ***edges) {
  char line[LINE_LEN], vertex_name[MAX_VERTEX_NAME_LEN], *working_line;
  int count = 0, vertex_label, vertices_arr_size = STARTING_VERTICES_ARR_SIZE;
  FILE *source_file;

  /* ošetření vstupu */
  if(source_file_ptr == NULL || *source_file_ptr == NULL || vertex_count == NULL || starting_vertex == NULL || vertices == NULL || edges == NULL) {
    terminate(RUNTIME_ERR_MSG);
  }

  source_file = *source_file_ptr;

  table = (hash_table *) malloc(sizeof(hash_table));  /* alokujeme místo pro tabulku, při nezdaru ukončíme program, při úspěchu zvýšíme počet alokovaných bloků */
  if(table == NULL) {
    terminate(RUNTIME_ERR_MSG);
  }
  increase_block_count();

  initialize_hash_table(table);  /* nastavíme tabulce výchozí hodnoty */

  *vertices = (int *) malloc(STARTING_VERTICES_ARR_SIZE * sizeof(int));  /* připrvíme pole vrcholů o startovní velikosti */
  if(*vertices == NULL) {
    free_table_and_terminate();
  }
  increase_block_count();

  skip_header(line, source_file);  /* přeskočíme řádky bez relevantních informací */
  working_line = get_and_trim_line(line, source_file);  /* načteme a ořízneme další řádku */

  while(strcmp(working_line, EDGES_SEPARATOR) != 0) {  /* dokud nenarazíme na komentář označující začátek hran */
    parse_vertex(&vertex_label, vertex_name, working_line);  /* získám informace o vrcholu a zvýšíme jejich počet */
    count++;

    if(vertex_label == INPUT_VERTEX) {  /* pokud je vrchol vstupní tak zaznamenáme jeho index v poli */
      *starting_vertex = count - 1;
    }

    if(count > vertices_arr_size) {  /* pokud je počet prvků větší než velikost pole tak ho dvkrát zvětšíme */
      expand_array(&vertices_arr_size, vertices);
    }
    (*vertices)[count - 1] = (vertex_label == INPUT_VERTEX) ? ORDINARY_VERTEX : vertex_label;  /* uložíme do pole označení, jestli je vrchol výstpuní */

    add_to_table(&table, vertex_name, count - 1);  /* uložíme do hashovací tabulky jméno vrcholu a jeho index v poli */

    working_line = get_and_trim_line(line, source_file);
  }

  *vertex_count = count;  /* uložíme počet vrcholů */

  load_edges(source_file_ptr, count, edges);  /* dále načteme také hrany */

  free_hash_table(table);  /* uvolníme tabulku a uzavřeme sooubor */
  fclose(source_file);
  source_file = NULL;
}
