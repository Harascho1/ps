#include "mpi.h"
#include <stdio.h>
#define N 4

int main(int argc, char *argv[]) {
  int rank, size;
  int row, col;
  int A[N][N] = {
      {1, 2, 3, 4},
      {2, 3, 4, 5},
      {3, 4, 5, 6},
      {4, 5, 6, 7},
  };

  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);

  if (N * N != size) {
    if (rank == 0)
      printf("Broj procesa mora biti N x N\n");
    MPI_Finalize();
    return -1;
  }

  row = rank / N;
  col = rank % N;

  MPI_Comm donjiTrougao;
  int color;
  color = (row >= col) ? 1 : MPI_UNDEFINED;

  MPI_Comm_split(MPI_COMM_WORLD, color, rank, &donjiTrougao);

  int diagonala[N] = {0};
  if (donjiTrougao != MPI_COMM_NULL) {
    int new_rank, new_size;
    MPI_Comm_rank(donjiTrougao, &new_rank);

    for (int i = 0; i < N; i++) {
      int vrednost;
      vrednost = (rank == 0) ? A[i][i] : 0;
      MPI_Bcast(&vrednost, 1, MPI_INT, 0, donjiTrougao);
      diagonala[i] = vrednost;
    }
    if (rank == 12) {
      printf("rank=%d new_rank=%d diagonala: ", rank, new_rank);
      for (int i = 0; i < N; i++)
        printf("%d ", diagonala[i]);

      printf("\n");
    }

    MPI_Comm_free(&donjiTrougao);
  }
  if (rank == 12) {
    printf("rank=%d diagonala: ", rank);
    for (int i = 0; i < N; i++)
      printf("%d ", diagonala[i]);

    printf("\n");
  }

  MPI_Finalize();
  return 0;
}
