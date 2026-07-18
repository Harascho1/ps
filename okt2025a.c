#include "mpi.h"
#include <stdio.h>
#include <string.h>

#define N 10
#define K 6
#define M 8

int main(int argc, char *argv[]) {
  int rank, size;

  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);

  if (K % size != 0) {
    printf("Greska\n");
    MPI_Finalize();
    return 1;
  }

  int A[N][K], B[K][M], C[N][M];

  int cols_per_proc = K / size;
  // int locA[N * cols_per_proc];
  int locA[N][cols_per_proc];
  int locB[cols_per_proc][M];

  if (rank == 0) {
    for (int i = 0; i < N; i++) {
      for (int j = 0; j < K; j++) {
        A[i][j] = i + j;
      }
    }
    for (int i = 0; i < K; i++) {
      for (int j = 0; j < M; j++) {
        B[i][j] = i + j;
      }
    }
  }

  MPI_Datatype colA, tmp_colA;
  MPI_Type_vector(N, 1, K, MPI_INT, &tmp_colA);
  MPI_Type_create_resized(tmp_colA, 0, sizeof(int), &colA);
  MPI_Type_commit(&colA);

  MPI_Datatype rowB;
  MPI_Type_contiguous(M, MPI_INT, &rowB);
  MPI_Type_commit(&rowB);

  MPI_Scatter(&A[0][0], cols_per_proc, colA, locA, N * cols_per_proc, MPI_INT,
              0, MPI_COMM_WORLD);
  MPI_Scatter(&B[0][0], cols_per_proc, rowB, locB, M * cols_per_proc, MPI_INT,
              0, MPI_COMM_WORLD);

  int locC[N][M];
  memset(locC, 0, sizeof(locC));

  for (int i = 0; i < N; i++) {
    for (int j = 0; j < M; j++) {
      for (int t = 0; t < cols_per_proc; t++) {
        locC[i][j] += locA[t][i] * locB[t][j];
      }
    }
  }

  MPI_Reduce(locC, C, N * M, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

  if (rank == 0) {
    for (int i = 0; i < N; i++) {
      for (int j = 0; j < M; j++)
        printf("\t %d", C[i][j]);
      printf("\n");
    }
  }

  MPI_Type_free(&colA);
  MPI_Type_free(&rowB);
  MPI_Type_free(&tmp_colA);
  MPI_Finalize();

  return 0;
}
