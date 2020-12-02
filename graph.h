/*

    Generování Konečného Automatu

    Hlavičkový soubor graph.h
    Detailní informace viz graph.c

    Dialekt : ANSI C
    Překladač: jakýkoliv ANSI C-kompatibilní

*/

#ifndef _GRAPH_H
#define _GRAPH_H

/* ____________________________________________________________________________

    Definované Konstanty
   ____________________________________________________________________________
*/

/* ____________________________________________________________________________

    Definice Strukutr
   ____________________________________________________________________________
*/
/* ____________________________________________________________________________

    Struktura reprezentuje hranu automatu.
   ____________________________________________________________________________
*/
typedef struct the_edge {
  int dest_index;  /* index vrcholu, do kterého hrana vede */
  char edge_char;  /* znak svázaný s touotu hranou automatu */
  struct the_edge *next;  /* další prvek ve spojovém seznamu hran */
} edge;

/* ____________________________________________________________________________

    Prototypy Funkcí
   ____________________________________________________________________________
*/


#endif
