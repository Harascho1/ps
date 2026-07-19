#include "mpi.h"
#include <math.h>
#include <stdio.h>

#define N 16

int main(int argc, char* argv[]) {
  int p, rank;

  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &p);

  int sqrt_p = (int)round(sqrt((double)p));

  if (sqrt_p * sqrt_p != p) {
    printf("NEMOZE\n");
    MPI_Finalize();
    // izgleda da mora da bude 0 jer mpi vristi ako ne vrati 0
    return 0;
  }

  int X[N][N];
  if (rank == 0) {
    for (int i = 0; i < N; i++) {
      for (int j = 0; j < N; j++) {
        X[i][j] = i * 2 + 1;
      }
    }
    for (int i = 0; i < N; i++) {
      for (int j = 0; j < N; j++) {
        printf("%d\t", X[i][j]);
      }
      printf("\n");
    }
  }

  int row = rank / sqrt_p;
  int col = rank % sqrt_p;

  MPI_Comm prva_kolona;
  int color = (col == 0) ? 1 : MPI_UNDEFINED;

  MPI_Comm_split(MPI_COMM_WORLD, color, rank, &prva_kolona);

  if (prva_kolona != MPI_COMM_NULL) {
    int new_rank;
    MPI_Comm_rank(prva_kolona, &new_rank);

    int np = N / sqrt_p;
    int diag[N] = {0};

    if (rank == 0 && new_rank == 0) {
      for (int i = 0; i < N; i++) {
        diag[i] = X[i][i];
      }
    }

    int local_diag[np];
    MPI_Scatter(
      diag, np, MPI_INT, local_diag, np, MPI_INT, 0, prva_kolona
    );

    int proizvod = 1;
    for (int i = 0; i < np; i++) {
      proizvod *= local_diag[i];
    }
    printf(
      "World rank=%d, rank u novom komunikatoru=%d, proizvod=%d\n",
      rank, new_rank, proizvod
    );

    MPI_Comm_free(&prva_kolona);
  }

  MPI_Finalize();
  return 0;
}
