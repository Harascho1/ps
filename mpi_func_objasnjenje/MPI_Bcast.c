#include "mpi.h"
/*
 * NOTE:
 * Ova funkcija se koristi samo kad saljemo ceo podatak. Ne secemo ga
 * u kolone, vrste ili u donji ili gornji trougao.
 */

void* data;

int main() {
  int res = MPI_Bcast(data, 1, MPI_INT, 0, MPI_COMM_WORLD);
  /*
   * 1. argument je podatak koji saljemo
   * 2. koliko puta ga saljemo (uglavnom je 1 u slucaju MPI_Bcast-a)
   * 3. MPI tip podatka, obicno MPI_INT ili MPI_FLOAT
   * 4. koji proces salje, obicno je to 0 (master) jer ga on i
   * inicijalizuje
   * 5. comm kanal
   */

  /*
   * Ne treba nikakav MPI_Recv i ovu komandu svi izvrsavaju
   * BUG: Ne stavljati ovo u if (rank == 0)
   */
}
