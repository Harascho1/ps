#include "mpi.h"

#define N 5
#define K 4
#define S 3

int main(int argc, char *argv[]) {
  int rank, proc;
  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &proc);

  int A[N][S];
  int B[S][K];
  int C[N][K];

  if (rank == 0) {
    for (int i = 0; i < N; i++)
      for (int j = 0; j < S; j++)
        A[i][j] = i + j + 1;

    for (int i = 0; i < S; i++)
      for (int j = 0; j < K; j++)
        B[i][j] = i + j + 1;

    MPI_broadcast()
  } else {
  }
}
