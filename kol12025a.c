// KOL12025a
#include "mpi.h"
#include <stdio.h>
#define N 4
#define K 21

int main(int argc, char *argv[]) {
  int mat_A[N][K];
  int vec_b[K] = {0};
  int vec_c[N] = {0};

  int rank, size;
  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);

  if (K % size != 0) {
    printf("broj procesa nije deljiv sa brojem kolona\n");
    MPI_Finalize();
    return -1;
  }

  if (rank == 0) {
    // inicijalizacija matrice
    for (int i = 0; i < N; i++) {
      for (int j = 0; j < K; j++) {
        mat_A[i][j] = i + j;
      }
    }
    // inicijalizacija vektora
    for (int i = 0; i < K; i++) {
      vec_b[i] = i + 1;
    }
  }

  int kp = K / size;

  // 1.
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
  MPI_Scatter(mat_A, 1, block_resized, local_A, kp * N, MPI_INT, 0,
              MPI_COMM_WORLD);

  int local_b[kp];
  if (rank == 0) {
    for (int dest = 0; dest < size; dest++) {
      if (dest == 0) {
        for (int t = 0; t < kp; t++) {
          local_b[t] = vec_b[dest + t * size];
        }
      } else {
        MPI_Datatype vecb_type;
        MPI_Type_vector(kp, 1, size, MPI_INT, &vecb_type);
        MPI_Type_commit(&vecb_type);
        MPI_Send(&vec_b[dest], 1, vecb_type, dest, 77, MPI_COMM_WORLD);
        MPI_Type_free(&vecb_type);
      }
    }
  } else {
    MPI_Recv(local_b, kp, MPI_INT, 0, 77, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
  }

  int local_c[N] = {0};
  for (int i = 0; i < kp; i++) {
    for (int j = 0; j < N; j++) {
      local_c[j] += local_A[i][j] * local_b[i];
    }
  }

  MPI_Reduce(local_c, vec_c, N, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

  if (rank == 0) {
    printf("Rezultujuci vektor c:\n");
    for (int i = 0; i < N; i++)
      printf("%d\t", vec_c[i]);
    printf("\n");
  }

  MPI_Type_free(&col_type);
  MPI_Type_free(&col_resized);
  MPI_Type_free(&block_type);
  MPI_Type_free(&block_resized);

  MPI_Finalize();

  return 0;
}
