/*

    Generování Konečného Automatu

    Modul loader.c
    <popis>


    Dialekt : ANSI C
    Překladač: jakýkoliv ANSI C-kompatibilní

*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "loader.h"
#include "graph.h"
#include "err_manager.h"
#include "defs.h"
#include "memory_observer.h"
#include "hash_table.h"

/* ____________________________________________________________________________

    Globální Proměnné Modulu
   ____________________________________________________________________________
*/
hash_table *table = NULL;

/* ____________________________________________________________________________

    static char *left_trim(char *line)

    <popis>
   ____________________________________________________________________________
*/
static char *left_trim(char *line) {
  if(line == NULL) {
    return line;
  }

  while(isspace(*line)) {
    line++;
  }
  return line;
}

/* ____________________________________________________________________________

    static void parse_vertex(int *vertex_label, char *vertex_name, char *line)

    <popis>
   ____________________________________________________________________________
*/
static void parse_vertex(int *vertex_label, char *vertex_name, char *line) {
  int i;

  /* ošetření vstupu */
  if(vertex_label == NULL || vertex_name == NULL || line == NULL) {
    terminate(RUNTIME_ERR_MSG);
  }
  memset((void *) vertex_name, '\0', MAX_VERTEX_NAME_LEN);

  for(i = 0; i < (int) strlen(line); i++) {
    if(line[i] == ';') {
      line[i] = '\0';
      strcpy(vertex_name, line);
      *vertex_label = ORDINARY_VERTEX;
      return;
    }

    if(line[i] == '[') {
      line[i] = '\0';
      strcpy(vertex_name, line);

      if(line[i + LABEL_DIST] == 's') {
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

    static void get_and_trim_line(char *line[], FILE *source_file)

    <popis>
   ____________________________________________________________________________
*/
static char *get_and_trim_line(char *line, FILE *source_file) {
  if(source_file == NULL) {
    terminate(RUNTIME_ERR_MSG);
  }

  if(fgets(line, LINE_LEN, source_file) == NULL) {  /* načtení vrcholů grafu */
    terminate(RUNTIME_ERR_MSG);
  }
  return left_trim(line);  /* ořízneme bílé znaky */
}

/* ____________________________________________________________________________

    static void skip_header(char line[], FILE *source_file)

    <popis>
   ____________________________________________________________________________
*/
static void skip_header(char line[], FILE *source_file) {
  const int HEADER_LINES_COUNT = 3;
  int i;

  if(source_file == NULL) {
    terminate(RUNTIME_ERR_MSG);
  }

  for(i = 0; i < HEADER_LINES_COUNT; i++) {  /* přeskočení hlavičky souboru, složené závorky a komentáře */
    if(fgets(line, LINE_LEN, source_file) == NULL) {
      terminate(RUNTIME_ERR_MSG);
    }
  }
}

/* ____________________________________________________________________________

    static void expand_array(int *vertices_arr_size, int **vertices)

    <popis>
   ____________________________________________________________________________
*/
static void expand_array(int *vertices_arr_size, int **vertices) {
  int *new_arr;

  if(vertices_arr_size == NULL || vertices == NULL) {
    terminate(RUNTIME_ERR_MSG);
  }

  *vertices_arr_size *= 2;
  new_arr = (int *) realloc(*vertices, *vertices_arr_size * sizeof(int));

  if(new_arr == NULL) {
    terminate(RUNTIME_ERR_MSG);
  }

  *vertices = new_arr;
}

/* ____________________________________________________________________________

    void load_graph(FILE **source_file_ptr, int *vertex_count, int *starting_vertex, int **vertices, edge ***edges)

    <popis>
   ____________________________________________________________________________
*/
void load_edges(FILE **source_file_ptr, int vertex_count, edge ***edges) {
  *edges = (edge **) malloc(vertex_count * sizeof(edge *));

  if(*edges == NULL) {
    free_hash_table(table);
    terminate(RUNTIME_ERR_MSG);
  }
  increase_block_count();

  
}

/* ____________________________________________________________________________

    void load_graph(FILE **source_file_ptr, int *vertex_count, int *starting_vertex, int **vertices, edge ***edges)

    <popis>
   ____________________________________________________________________________
*/
void load_graph(FILE **source_file_ptr, int *vertex_count, int *starting_vertex, int **vertices, edge ***edges) {
  char line[LINE_LEN], vertex_name[MAX_VERTEX_NAME_LEN], *working_line;
  int count = 0, vertex_label, vertices_arr_size = STARTING_VERTICES_ARR_SIZE;
  FILE *source_file = *source_file_ptr;

  /* ošetření vstupu */
  if(source_file == NULL || vertex_count == NULL || starting_vertex == NULL || vertices == NULL || edges == NULL) {
    terminate(RUNTIME_ERR_MSG);
  }

  table = (hash_table *) malloc(sizeof(hash_table));
  if(table == NULL) {
    terminate(RUNTIME_ERR_MSG);
  }
  increase_block_count();

  initialize_hash_table(table);

  *vertices = (int *) malloc(STARTING_VERTICES_ARR_SIZE * sizeof(int));
  if(*vertices == NULL) {
    free_hash_table(table);
    terminate(RUNTIME_ERR_MSG);
  }
  increase_block_count();

  skip_header(line, source_file);
  working_line = get_and_trim_line(line, source_file);

  while(strcmp(working_line, EDGES_SEPARATOR) != 0) {  /* dokud nenarazíme na komentář označující začátek hran */
    parse_vertex(&vertex_label, vertex_name, working_line);
    count++;

    if(vertex_label == INPUT_VERTEX) {
      *starting_vertex = count - 1;
    }

    if(count > vertices_arr_size) {
      expand_array(&vertices_arr_size, vertices);
    }
    (*vertices)[count - 1] = (vertex_label == INPUT_VERTEX) ? ORDINARY_VERTEX : vertex_label;  /* uložíme do pole označení, jestli je vrchol výstpuní */

    add_to_table(&table, vertex_name, count - 1);

    working_line = get_and_trim_line(line, source_file);
  }

  *vertex_count = count;

  load_edges(source_file_ptr, count, edges);

  fclose(source_file);
  source_file = NULL;
}
