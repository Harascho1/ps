#include "mpi.h"
/*
 * NOTE:
 * MPI_Gather je GRUPNA operacija.
 * Najcesce se koriusti sa Datatype i to onim koji je !resized!
 * Ovo izvrsava svaki proces!
 */

#define N 4
#define M 5

int count;
int** data;

int main() {
  int* localData;
  int rank, size;
  int np = size / N;

  MPI_Datatype col_c_resized;
  MPI_Gather(
    localData, M * np, MPI_INT, data, 1, col_c_resized, 0,
    MPI_COMM_WORLD
  );
  /*
   * 1. argument je podatak koji saljemo taj podatak smo obicno pre
   * toga izracunali (mnozenje dela matrice)
   * 2. koliko podataka sukcesivno od pokazivaca
   * 3. MPI tip podatka, mada moze da bude i Datatype i cesce je
   * Datatype
   * 4. argument je podatak u koji cemo da stavlamo parcice, obicno je
   * data cela matrica dok je localData mali odsecak te matrice
   * 5. koliko podataka sukcesivno od pokazivaca
   * 6. MPI tip podatka, mada moze da bude i Datatype i cesce je
   * Datatype
   * 7. u kom procesu ce da se cuva rezultat 'data'
   * 6. comm kanal
   */

  /*
   * NOTE:
   * Cim se zavrsi sljanje dobra praksa je odmah obrisati tip i sve
   * njegove pomocne tipove
   */
  MPI_Type_free(&col_c_resized);
}
