/*

    Generování Konečného Automatu

    Hlavičkový soubor loader.h
    Detailní informace viz loader.c

    Dialekt : ANSI C
    Překladač: jakýkoliv ANSI C-kompatibilní

*/

#ifndef _LOADER_H
#define _LOADER_H

#include "graph.h"
#include <stdio.h>

/* ____________________________________________________________________________

    Definované Konstanty
   ____________________________________________________________________________
*/
#define LINE_LEN 50  /* maximá délka řádky z načítaného soubour*/
#define EDGES_SEPARATOR "//edges\n"  /* komentář označující začátek výčtu hran v souboru */
#define MAX_VERTEX_NAME_LEN 11
#define ORDINARY_VERTEX 0
#define OUTPUT_VERTEX 1
#define INPUT_VERTEX 2
#define STARTING_VERTICES_ARR_SIZE 5
#define LABEL_DIST 7


/* ____________________________________________________________________________

    Prototypy Funkcí
   ____________________________________________________________________________
*/
void load_graph(FILE **source_file_ptr, int *vertex_count, int *starting_vertex, int **vertices, edge ***edges);

#endif
