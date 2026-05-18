#include <mpi.h>
#include <stdio.h>

int main(int argc, char **argv) {
  int rank, size;

  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);

  // Program zahteva bar 2 procesa da bi radio (jedan šalje, drugi prima)
  if (size < 2) {
    if (rank == 0) {
      printf("Greska: Pokreni program sa najmanje 2 procesa (npr. mpirun -np 2 "
             "./program)\n");
    }
    MPI_Finalize();
    return 0;
  }

  // Definisanje parametara za podniz
  int array_size[2] = {5, 5}; // Ukupna veličina matrice 5x5
  int subarray_size[2] = {
      2, 3}; // Veličina podniza (2 reda, 3 kolone) - TVOJ ZAHTEV
  int subarray_start[2] = {
      1, 1}; // Početne koordinate podniza (indeksi kreću od 0)

  // Deklaracija matrice. Koristimo statički niz zbog kontinualne memorije.
  double array[5][5];

  // Inicijalizacija matrice: Proces 0 puni matricu nulama, a Proces 1
  // jedinicama
  for (int i = 0; i < array_size[0]; i++) {
    for (int j = 0; j < array_size[1]; j++) {
      array[i][j] = (double)rank;
    }
  }

  // Kreiranje novog tipa podataka (podniza)
  MPI_Datatype subtype;
  MPI_Type_create_subarray(2, array_size, subarray_size, subarray_start,
                           MPI_ORDER_C, MPI_DOUBLE, &subtype);
  MPI_Type_commit(&subtype);

  // Komunikacija
  if (rank == 0) {
    // Proces 0 prima podniz od procesa 1
    MPI_Recv(&array[0][0], 1, subtype, 1, 123, MPI_COMM_WORLD,
             MPI_STATUS_IGNORE);
    double tmp[5][5] = {};
    for (int i = 0; i < array_size[0]; i++) {
      for (int j = 0; j < array_size[1]; j++) {
        tmp[i][j] = (double)-1;
      }
    }
    MPI_Recv(&tmp[0][0], 1, subtype, 1, 123, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

    // Ispis rezultata nakon primanja
    printf("Rank 0: Izlazna matrica nakon primanja podniza:\n");
    for (int i = 0; i < array_size[0]; i++) {
      for (int j = 0; j < array_size[1]; j++) {
        printf("%.1f  ", array[i][j]);
      }
      printf("\n");
    }
    for (int i = 0; i < array_size[0]; i++) {
      for (int j = 0; j < array_size[1]; j++) {
        printf("%.1f  ", tmp[i][j]);
      }
      printf("\n");
    }
  } else if (rank == 1) {
    // Proces 1 šalje svoj podniz (koji se sastoji od jedinica) procesu 0
    MPI_Send(&array[0][0], 1, subtype, 0, 123, MPI_COMM_WORLD);
    MPI_Send(&array[0][0], 1, subtype, 0, 123, MPI_COMM_WORLD);
  }

  // Oslobađanje tipa i kraj
  MPI_Type_free(&subtype);
  MPI_Finalize();
  return 0;
}
