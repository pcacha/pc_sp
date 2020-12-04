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
#include "loader.h"

/* ____________________________________________________________________________

    Konstanty Modulu
   ____________________________________________________________________________
*/
#define GV_EXT ".gv"  /* konocovka .gv definovaná pro zvalidování vstupního souboru */
#define NO_ARG_MSG "An argument with the file name is missing.\n"  /* hláška oznamující chybějící argument s názem souboru */
#define INVALID_ARG_MSG "Given filename is not valid.\n"  /* hláška označující nevalidní soubor */
#define INVALID_SOURCE_MSG "The file cannot be open.\n"  /* hláška označuje, že soubour nelze otevřít */
#define MIN_FILE_NAME_LENGHT 4  /* minimální délka názvu sobour - *.gv */
#define GV_EXT_LENGHT 3  /* délka řetězce '.gv' */
#define GV_EXT ".gv"  /* přípona .gv */
#define OUTPUT_FILE_NAME "fsm.c"  /* název výstupního souboru */

/* ____________________________________________________________________________

    Definice Částí Zdrojového Kódu
   ____________________________________________________________________________
*/
#define GENERATED_INCLUDES "#include <stdlib.h>\n#include <stdio.h>\n#include <string.h>\n\n"
#define GENERATED_SUCCESS "void success() { \n\tprintf(\"Regular expression was accepted!\\n\");\n\texit(EXIT_SUCCESS);\n}\n\n"
#define GENERATED_FAILURE "void failure() { \n\tprintf(\"Regular expression was NOT accepted!\\n\");\n\texit(EXIT_FAILURE);\n}\n\n"
#define GENERATED_MAIN_HEADER "int main(int argc, char *argv[]) {\n"
#define GENERATED_VARIABLES "\tint i, string_len, state;\n\tchar *regex;\n\n"
#define GENERATED_MISSING_ARG "\tif(argc < 2) { \n\t\tprintf(\"Missing argument!\");\n\t\texit(EXIT_FAILURE);\n\t}\n\n"
#define GENERATED_GET_REGEX "\tregex = argv[1];\n"
#define GENERATED_GET_STRING_LEN "\tstring_len = strlen(regex);\n"
#define GENERATED_START_STATE "\tstate = %d;\n\n"
#define GENERATED_FOR "\tfor(i = 0; i <= string_len; i++) {\n"
#define GENERATED_SELECT_STATE "\t\tif(state == %d) {\n"
#define GENERATED_SELECT_STATE_ELSE "\t\telse if(state == %d) {\n"
#define GENERATED_DECIDE "\t\t\tif(i == string_len) {\n\t\t\t\t%s\n\t\t\t}\n\n"
#define GENERATED_SUCCESS_CALL "success();"
#define GENERATED_FAILURE_CALL "failure();"
#define GENERATED_USE_EDGE "\t\t\tif(regex[i] == '%c') {\n\t\t\t\tstate = %d;\n\t\t\t\tcontinue;\n\t\t\t}\n"
#define GENERATED_SELECT_STATE_END "\t\t}\n\n"
#define GENERATED_FOOTER "\t}\n\n\tfailure();\n\treturn EXIT_FAILURE;\n}"

/* ____________________________________________________________________________

    Globální Proměnné Modulu
   ____________________________________________________________________________
*/
FILE *source_file = NULL;  /* soubor s definicí automatu */
FILE *output_file = NULL;  /* soubor pro zápis kódu s reprezentací konečného automatu */
int vertex_count = 0;  /* označuje počet vrchlů grafu */
int starting_vertex = -1;  /* označuje index vstupního vrcholu automatu */
int *vertices = NULL;  /* pole vrchlů, hodnota označuje, zda je vrchol výstupní, 0 - vrchol není výstupní, 1 - vrchol je výstupní */
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

  extension_start_index = name_len - GV_EXT_LENGHT;  /* index počátku přípony */
  res = strcmp(&file_name[extension_start_index], GV_EXT);  /* provnáme příponu soubour s řetězcem .gv */
  return !res;
}

/* ____________________________________________________________________________

    void handle_input(int argc, char *argv[])

    Zpracuje argumenty z příkazové řádky. Pokud je předaný název souboru validní, tak ho otevře.
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

    void free_edge(edge *edge_ptr)

    Rekurzivní funkce na uvolňování zřetězených seznamů hran.
   ____________________________________________________________________________
*/
void free_edge(edge *edge_ptr) {
  if(edge_ptr == NULL) {
    return;
  }

  if(edge_ptr->next != NULL) { /* pokud má prvek potomka, musíme ho nejprve uvolnit */
    free_edge(edge_ptr->next);
  }

  free(edge_ptr);
  decrease_block_count();
}

/* ____________________________________________________________________________

    void shutdown()

    Funkce volaná při ukončení programu. Uvolní alokovanou paměť a uzavře souboury.
   ____________________________________________________________________________
*/
void shutdown() {
  int i;

  /* uzavření souborů */
  if(source_file != NULL) {
    fclose(source_file);
  }

  if(output_file != NULL) {
    fclose(output_file);
  }

  /* uvolnění pole vrcholů */
  if(vertices != NULL) {
    free(vertices);
    decrease_block_count();
  }

  /* uvolnění pole zřetězených seznamů a samotných seznamů */
  if(edges != NULL) {
    for(i = 0; i < vertex_count; i++) {
      if(edges[i] != NULL) {
        free_edge(edges[i]);
      }
    }

    free(edges);
    decrease_block_count();
  }
}

/* ____________________________________________________________________________

    void make_output_file()

    Funkce na základě načteného automatu vygeneruje zdrojový kód s reprezetací tohoto automatu v ANSI C.
   ____________________________________________________________________________
*/
void make_output_file() {
  int i;
  edge *cur;

  output_file = fopen(OUTPUT_FILE_NAME, "w");
  if(output_file == NULL) {  /* pokud nemáme kam zapisovat ukončíme program s chybou */
    terminate(RUNTIME_ERR_MSG);
  }

  /* necháme vypsat strukuru programu do souboru */
  fprintf(output_file, GENERATED_INCLUDES);
  fprintf(output_file, GENERATED_SUCCESS);
  fprintf(output_file, GENERATED_FAILURE);
  fprintf(output_file, GENERATED_MAIN_HEADER);
  fprintf(output_file, GENERATED_VARIABLES);
  fprintf(output_file, GENERATED_MISSING_ARG);
  fprintf(output_file, GENERATED_GET_REGEX);
  fprintf(output_file, GENERATED_GET_STRING_LEN);
  fprintf(output_file, GENERATED_START_STATE, starting_vertex);
  fprintf(output_file, GENERATED_FOR);

  for(i = 0; i < vertex_count; i++) {
    if(i == 0) {  /* výběr logiky na základě aktuálního satavu automatu */
      fprintf(output_file, GENERATED_SELECT_STATE, i);
    }
    else {
      fprintf(output_file, GENERATED_SELECT_STATE_ELSE, i);
    }

    /* pokud už byly zpracovány všechny znaky, vrátíme určíme zda byl řetězec akceptován na základě toho jestli je vrchol výstupní */
    fprintf(output_file, GENERATED_DECIDE, vertices[i] ? GENERATED_SUCCESS_CALL : GENERATED_FAILURE_CALL);

    /* do daného stavu automatu vypíšeme všechny jeho aktuální možné hrany */
    cur = edges[i];
    while(cur != NULL) {
      fprintf(output_file, GENERATED_USE_EDGE, cur->edge_char, cur->dest_index);
      cur = cur->next;
    }

    fprintf(output_file, "\n\t\t\t%s\n", GENERATED_FAILURE_CALL);
    fprintf(output_file, GENERATED_SELECT_STATE_END);
  }

  fprintf(output_file, GENERATED_FOOTER);
}

/* ____________________________________________________________________________

    Hlavní Program
   ____________________________________________________________________________
*/
int main(int argc, char *argv[]) {
  atexit(shutdown);  /* registrace funkce volané při ukončení programu */
  handle_input(argc, argv);  /* zpracuje parametr z příkazové řádky a ověří jeho validitu */
  load_graph(&source_file, &vertex_count, &starting_vertex, &vertices, &edges);  /* načte graf ze souboru */
  make_output_file(); /* vytvoří výstpní soubor s kódem v ANSIC C */

  printf("fsm.c was successfully created!");
  return EXIT_SUCCESS;
}
