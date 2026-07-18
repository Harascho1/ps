#include "mpi.h"
#include <stdio.h>

#define N 6

int main(int argc, char *argv[]) {
  int rank, size;
  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD, &size);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  if (size != N * N) {
    printf("nece\n");
    MPI_Finalize();
    return 0;
  }

  int A[N][N];
  int row = rank / N;
  int col = rank % N;

  int color = (col >= row) ? 1 : MPI_UNDEFINED;
  MPI_Comm donji;
  MPI_Comm_split(MPI_COMM_WORLD, color, rank, &donji);

  if (rank == 0) {
    for (int i = 0; i < N; i++) {
      for (int j = 0; j < N; j++) {
        A[i][j] = i + j;
      }
    }
  }

  if (donji != MPI_COMM_NULL) {
    int new_rank, new_size;
    MPI_Comm_rank(donji, &new_rank);
    MPI_Comm_size(donji, &new_size);

    MPI_Datatype diagonal, temp_diagonal;
    MPI_Type_vector(N, 1, N + 1, MPI_INT, &temp_diagonal);
    MPI_Type_create_resized(temp_diagonal, 0, sizeof(int), &diagonal);
    MPI_Type_commit(&diagonal);

    int lista_diagonal[N];
    MPI_Scatter(A, 1, diagonal, lista_diagonal, N, MPI_INT, 0, donji);
    printf("Rank %d (new_rank=%d, pozicija [%d,%d]): Primio dijagonale:\n",
           rank, new_rank, row, col);
    if (new_rank == 0) {
      for (int i = 0; i < N; i++) {
        printf("  lista_diagonal[%d] = %d\n", i, lista_diagonal[i]);
      }
    }

    MPI_Type_free(&diagonal);
    MPI_Type_free(&temp_diagonal);
  }

  MPI_Finalize();

  return 0;
}
