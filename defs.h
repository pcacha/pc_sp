/*

    Generování Konečného Automatu

    Hlavičkový soubor defs.h
    Hlavičkový soubor obsahuje konstanty sdílené napříč projektem

    Dialekt : ANSI C
    Překladač: jakýkoliv ANSI C-kompatibilní

*/

#ifndef _DEFS_H
#define _DEFS_H

/* ____________________________________________________________________________

    Sdílené Konstanty
   ____________________________________________________________________________
*/

#define TRUE 1   /* číselná reprezentace pravdy */
#define FALSE 0   /* číselná reprezentace nepravdy */
#define RUNTIME_ERR_MSG "An unexpected error has occured.\n"

/* ____________________________________________________________________________

    Struktura reprezentuje hranu automatu.
   ____________________________________________________________________________
*/
typedef struct the_edge {
  int dest_index;  /* index vrcholu, do kterého hrana vede */
  char edge_char;  /* znak svázaný s touotu hranou automatu */
  struct the_edge *next;  /* další prvek ve spojovém seznamu hran */
} edge;

#endif
