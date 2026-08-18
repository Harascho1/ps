#include "mpi.h"
/*
 * NOTE:
 * Ova funkcija se koristi u kombinaciji sa MPI_Recv i vrlo cesto sa
 * Datatype entitetom
 */

int** data;
int count;

int main() {
  int rank, size;
  int stride;
  int* localData;
  if (rank == 0)
    /* NOTE:
     * sljanje tj. MPI_Send OBICNO izvrsava master proces
     */
    for (int dest = 1; dest < size; dest++) {
      MPI_Send(
        &data[0][dest * stride], count, MPI_INT, dest, 0,
        MPI_COMM_WORLD
      );
      /*
       * 1. argument je podatak koji saljemo taj podatak uglavnom
       * 'secemo' koristeci offset
       * 2. koliko podataka sukcesivno od pokazivaca
       * 3. MPI tip podatka, mada moze da bude i Datatype
       * 4. ovde saljemo rank (redni broj procesa) koji dobija deo
       * podatka
       * 5. 'tag' neki glupavi
       * 6. comm kanal
       */

      // NOTE: kljucan deo
      for (int i = 0; i < 67; i++) {
        localData[i] = data[0][i];
      }
      /*
       * BUG:
       * posto dest krece od 1 (dest=1), podrazumeva se da ce se
       * pocetak cuvati u master procesu
       */
    }
  else {
    MPI_Recv(
      localData, count, MPI_INT, 0, 0, MPI_COMM_WORLD,
      MPI_STATUS_IGNORE
    );
    /*
     * 1. argument je podatak u koji cuvamo podatak koji nam je stigao
     * preko MPI_Send-a
     * 2. koliko podataka sukcesivno od pokazivaca
     * 3. MPI tip podatka, mada moze da bude i Datatype
     * 4. rank source-a tj. proces koji zove MPI_Send()
     * 5. 'tag' neki glupavi
     * 6. comm kanal
     * 7. ovo tek ne znam cemu sluzi
     */
  }
}
