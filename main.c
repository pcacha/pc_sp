/*

    Generování Konečného Automatu

    Modul main.c
    Modoul obsahuje startovní bod celého programu. Stará se o načtení parametru z příkazové
    řádky a skládá funkcionalitu programu dohromoady voláním funkcí příslušných modulů.


    Dialekt : ANSI C
    Překladač: jakýkoliv ANSI C-kompatibilní

*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "memory_observer.h"
#include "err_manager.h"
#include "defs.h"
#include "graph.h"
#include "loader.h"

/* ____________________________________________________________________________

    Konstanty Modulu
   ____________________________________________________________________________
*/
#define GV_EXT ".gv"  /* konocovka .gv definovaná pro zvalidování vstupního souboru */
#define NO_ARG_MSG "An argument with the file name is missing.\n"
#define INVALID_ARG_MSG "Given filename is not valid.\n"
#define INVALID_SOURCE_MSG "The file cannot be open.\n"
#define MIN_FILE_NAME_LENGHT 4
#define GV_EXT_LENGHT 3
#define GV_EXT ".gv"

/* ____________________________________________________________________________

    Globální Proměnné Modulu
   ____________________________________________________________________________
*/
FILE *source_file = NULL;  /* soubor s definicí automatu */
int vertex_count = 0;  /* označuje počet vrchlů grafu */
int starting_vertex = -1;  /* označuje index počátečního vrcholu automatu */
int *vertices = NULL;  /* pole vrchlů, hodnota označuje, zda je vrchol koncový, 0 - vrchol není koncový, 1 - vrchol je koncový */
edge **edges = NULL;  /* reprezentace hran automatu, ukazatel na ple ukazatelů na spojové seznamy hran vázané k danému vrcholu na základě indexu */

/* ____________________________________________________________________________

    int has_gv_extension(char *file_name)

    Funkce vrací, zda předaný název soubour obsahuje příponu 'gv'.
   ____________________________________________________________________________
*/
int has_gv_extension(char *file_name) {
  int name_len, extension_start_index = -1, res;

  if(file_name == NULL) {
    terminate(RUNTIME_ERR_MSG);
  }

  name_len = strlen(file_name);

  if(name_len < MIN_FILE_NAME_LENGHT) {
    return FALSE;  /* pokud je název soubuoru kratší než minimální délka názvu, můžeme vrátit false */
  }

  extension_start_index = name_len - GV_EXT_LENGHT;
  res = strcmp(&file_name[extension_start_index], GV_EXT);  /* provnáme příponu soubour s řetězcem .gv */
  return !res;
}

/* ____________________________________________________________________________

    void handle_input(int argc, char *argv[])

    <popis>
   ____________________________________________________________________________
*/
void handle_input(int argc, char *argv[]) {
  if(argc < 2) {  /* kontrola přítomnosti argumentu  */
    terminate_with_help(NO_ARG_MSG);
  }

  if(!has_gv_extension(argv[1])) {  /* kontrola zda předaný název sobuboru obsahuje 'gv' příponu */
    terminate_with_help(INVALID_ARG_MSG);
  }

  source_file = fopen(argv[1], "r");  /* pokus o otevření daného souboru */
  if(source_file == NULL) {  /* pokud se otevření nezdařilo, informujeme a ukončíme program */
    terminate_with_help(INVALID_SOURCE_MSG);
  }
}

/* ____________________________________________________________________________

    void shutdown()

    Funkce volaná při ukončení programu. Uvolní alokovanou paměť a uzavře souboury.
   ____________________________________________________________________________
*/
void shutdown() {

}

void print_edges() {
  edge *cur;
  int i;

  for(i = 0; i < vertex_count; i++) {
    cur = edges[i];

    while(cur != NULL) {
      printf("%d\n", i);
      printf("%d\n", cur->dest_index);
      printf("%c\n\n\n", cur->edge_char);
      cur = cur->next;
    }
  }
}

/* ____________________________________________________________________________

    Hlavní Program
   ____________________________________________________________________________
*/
int main(int argc, char *argv[]) {

  atexit(shutdown);  /* registrace funkce volané při ukončení programu */
  handle_input(argc, argv);  /* zpracuje parametr z příkazové řádky a ověří jeho validitu */
  load_graph(&source_file, &vertex_count, &starting_vertex, &vertices, &edges);  /* načte graf ze souboru */

  print_edges();

  printf("All passed!");
  return EXIT_SUCCESS;
}
