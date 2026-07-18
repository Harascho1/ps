#include "mpi.h"
#include <stdio.h>

#define N 5
#define K 8
#define M 5

int main(int argc, char *argv[]) {

  int rank, size;
  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);

  if (K % size) {
    printf("NEMOZe\n");
    MPI_Finalize();
    return -1;
  }

  int A[N][K];
  int B[K][M];
  int C[N][M];

  if (rank == 0) {
    for (int i = 0; i < N; i++) {
      for (int j = 0; j < K; j++) {
        A[i][j] = i + j;
      }
    }
    for (int i = 0; i < K; i++) {
      for (int j = 0; j < M; j++) {
        B[i][j] = i + j + 1;
      }
    }
    printf("Rezultujuca matrica A:\n");
    for (int i = 0; i < N; i++) {
      for (int j = 0; j < K; j++)
        printf("%d\t", A[i][j]);
      printf("\n");
    }
    printf("Rezultujuca matrica B:\n");
    for (int i = 0; i < K; i++) {
      for (int j = 0; j < M; j++)
        printf("%d\t", B[i][j]);
      printf("\n");
    }
  }

  int kp = K / size;

  MPI_Datatype col_type;
  MPI_Type_vector(N, 1, K, MPI_INT, &col_type);
  MPI_Type_commit(&col_type);

  MPI_Datatype col_resized;
  MPI_Type_create_resized(col_type, 0, sizeof(int), &col_resized);
  MPI_Type_commit(&col_resized);

  MPI_Datatype block_type;
  MPI_Type_vector(kp, 1, size, col_resized, &block_type);
  MPI_Type_commit(&block_type);

  MPI_Datatype block_resized;
  MPI_Type_create_resized(block_type, 0, sizeof(int), &block_resized);
  MPI_Type_commit(&block_resized);

  int local_A[kp][N];
  MPI_Scatter(A, 1, block_resized, local_A, kp * N, MPI_INT, 0, MPI_COMM_WORLD);

  MPI_Datatype blockB_type;
  MPI_Type_vector(kp, M, size * M, MPI_INT, &blockB_type);
  MPI_Type_commit(&blockB_type);

  MPI_Datatype blockB_resized;
  MPI_Type_create_resized(blockB_type, 0, sizeof(int), &blockB_resized);
  MPI_Type_commit(&blockB_resized);

  int local_B[kp][M];
  MPI_Scatter(B, 1, blockB_resized, local_B, kp * M, MPI_INT, 0,
              MPI_COMM_WORLD);

  int local_C[N][M];
  for (int i = 0; i < N; i++) {
    for (int j = 0; j < M; j++) {
      local_C[i][j] = 0;
    }
  }

  for (int index = 0; index < kp; index++) {
    for (int i = 0; i < N; i++) {
      for (int j = 0; j < M; j++) {
        local_C[i][j] += local_A[index][i] * local_B[index][j];
      }
    }
  }

  MPI_Reduce(local_C, C, N * M, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

  if (rank == 0) {
    printf("Rezultujuca matrica C:\n");
    for (int i = 0; i < N; i++) {
      for (int j = 0; j < M; j++)
        printf("%d\t", C[i][j]);
      printf("\n");
    }
  }

  MPI_Type_free(&col_type);
  MPI_Type_free(&col_resized);
  MPI_Type_free(&block_type);
  MPI_Type_free(&block_resized);
  MPI_Type_free(&blockB_type);
  MPI_Type_free(&blockB_resized);

  MPI_Finalize();
  return 0;
}
