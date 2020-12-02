/*

    Generování Konečného Automatu

    Modul memory_observer.c
    Modul zajišťuje počítání aktuálně obsazených bloků paměti.
    Na konci programu by měl být počet 0.


    Dialekt : ANSI C
    Překladač: jakýkoliv ANSI C-kompatibilní

*/

#include "memory_observer.h"

/* ____________________________________________________________________________

    Globální Proměnné Modulu
   ____________________________________________________________________________
*/
int allocated_blocks_count = 0;  /* počet alokovaných bloků paměti */

/* ____________________________________________________________________________

    void increase_block_count()

    Zvýší počet alokovaných bloků paměti o jedna.
   ____________________________________________________________________________
*/
void increase_block_count() {
  allocated_blocks_count++;
}

/* ____________________________________________________________________________

    void decrease_block_count()

    Sníží počet alokovaných bloků paměti o jedna.
   ____________________________________________________________________________
*/
void decrease_block_count() {
  allocated_blocks_count--;
}

/* ____________________________________________________________________________

    int get_block_count()

    Vrátí počet alokovaných bloků paměti.
   ____________________________________________________________________________
*/
int get_block_count() {
  return allocated_blocks_count;
}
