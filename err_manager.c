/*

    Generování Konečného Automatu

    Modul err_manager.c
    Modul se zaměřuje na zpracování chyb vzniklých za běhu programu.
    Obahuje funkce pro vypisování chybových hlásek, použití a ukončení programu.


    Dialekt : ANSI C
    Překladač: jakýkoliv ANSI C-kompatibilní

*/

#include <stdlib.h>
#include <stdio.h>
#include "err_manager.h"

/* ____________________________________________________________________________

    static void help()

    Vypíše krátkou nápovědu o tom, jak program používat.
   ____________________________________________________________________________
*/
static void help() {
  printf("\nUsage:\n");
  printf("The program is started with one mandatory argument - a name of a file with a 'gv' extension.\n");
  printf("\t<app>.exe <file_name>.gv\n");
  printf("The output is a programm in C that needs to be compiled. Then it can be started with a regular expression as a parameter.\n");
  printf("\t<automaton>.exe <regex>\n");
}

/* ____________________________________________________________________________

    void terminate(char msg[])

    Vypíše chybovou hlášku a ukončí program s chybovým kódem.
   ____________________________________________________________________________
*/
void terminate(char msg[]) {
  if(msg != NULL) {
    printf("%s\n", msg);
  }

  exit(EXIT_FAILURE);
}

/* ____________________________________________________________________________

    void terminate_with_help(char msg[])

    Vypíše chybovou hlášku, vypíše návod k použití a ukončí program s chybovým kódem.
   ____________________________________________________________________________
*/
void terminate_with_help(char msg[]) {
  if(msg != NULL) {
    printf("%s\n", msg);
  }

  help();
  exit(EXIT_FAILURE);
}
