/*

    Generování Konečného Automatu

    Hlavičkový soubor loader.h
    Detailní informace viz loader.c

    Dialekt : ANSI C
    Překladač: jakýkoliv ANSI C-kompatibilní

*/

#ifndef _LOADER_H
#define _LOADER_H

#include <stdio.h>
#include "defs.h"


/* ____________________________________________________________________________

    Definované Konstanty
   ____________________________________________________________________________
*/
#define LINE_LEN 50  /* maximá délka řádky z načítaného soubour*/
#define EDGES_SEPARATOR "//edges\n"  /* komentář označující začátek výčtu hran v souboru */
#define FILE_END_CHAR '}'  /* znak ukončující definici hran */
#define MAX_VERTEX_NAME_LEN 11  /* maximální velikost řetězce reprezetujícího název vrcholu */
#define ORDINARY_VERTEX 0  /* vrchol není ani vstupní ani výstpuní */
#define OUTPUT_VERTEX 1  /* vrchol je výstpní */
#define INPUT_VERTEX 2  /* vrchol je vstupní */
#define STARTING_VERTICES_ARR_SIZE 5  /* počáteční velikost pole vrcholů */
#define LABEL_DIST 7  /* počet znaků reprezetující popisek */
#define ARROW_LEN 2  /* počet znaků šipky uvedené v definici hrany */


/* ____________________________________________________________________________

    Prototypy Funkcí
   ____________________________________________________________________________
*/
void load_graph(FILE **source_file_ptr, int *vertex_count, int *starting_vertex, int **vertices, edge ***edges);

#endif
