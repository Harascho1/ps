#include "mpi.h"
#include <stdio.h>
#include <string.h>

#define M 3
#define N 4
#define K 3

int main(int argc, char** argv) {
  int rank, size;

  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD, &size);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  // Provera da li mozemo da raspodelimo kolone po procesima
  if (N % size != 0) {
    printf("p | N!\n, p = %d, N = %d\n", size, N);
    return 0;
  }
  // Broj kolona koje dobija svaki proces
  int np = N / size;

  int A[M][K], B[K][N];
  // A x B => C => C[M][N]
  int C[M][N];

  // Lokalne B kolone
  int local_B_cols[K][np];

  // Lokalno izracunavanje koje radi svaki proces (deo cele C matrice)
  int local_C[M][np];

  // incijalizacija vrednosti obe matrice (A i B)
  if (rank == 0) {
    for (int i = 0; i < M; i++) {
      for (int j = 0; j < K; j++) {
        A[i][j] = i + j;
      }
    }

    for (int i = 0; i < K; i++) {
      for (int j = 0; j < N; j++) {
        B[i][j] = i + j;
      }
    }
  }

  // NOTE: master proces (0) salje svakom procesu celu matricu A
  MPI_Bcast(A, M * K, MPI_INT, 0, MPI_COMM_WORLD);
  // Ovde nam ne treba lokalna matrica za svaki proces
  // Trebala bi nam ako bi morali da koristimo Send/Recv

  // Elementi svih kolona(n/p kolona) matrice B koji se salju svakom
  // procesu salju se odjendom direktno iz matrice B.

  // NOTE: Kada saljemo kolone:
  MPI_Datatype col_b_type;
  MPI_Type_vector(K, np, N, MPI_INT, &col_b_type);
  MPI_Type_commit(&col_b_type);

  if (rank == 0) {
    for (int i = 0; i < K; i++) {
      for (int j = 0; j < np; j++) {
        local_B_cols[i][j] = B[i][j];
      }
    }
    for (int dest = 1; dest < size; dest++) {
      MPI_Send(
        &B[0][dest * np], 1, col_b_type, dest, 0, MPI_COMM_WORLD
      );
    }
  } else {
    MPI_Recv(
      local_B_cols, K * np, MPI_INT, 0, 0, MPI_COMM_WORLD,
      MPI_STATUS_IGNORE
    );
  }

  // Kada zavrsimo sa tipom samo pozovemo free
  MPI_Type_free(&col_b_type);

  memset(local_C, 0, sizeof(int) * M * np);
  for (int i = 0; i < M; i++) {
    for (int j = 0; j < np; j++) {
      for (int k = 0; k < K; k++) {
        local_C[i][j] += A[i][k] * local_B_cols[k][j];
      }
    }
  }

  // vracamo rezultate u MASTER proces
  MPI_Datatype col_c_type, col_c_resized;
  MPI_Type_vector(M, np, N, MPI_INT, &col_c_type);
  MPI_Type_commit(&col_c_type);

  MPI_Type_create_resized(
    col_c_type, 0, np * sizeof(int), &col_c_resized
  );
  MPI_Type_commit(&col_c_resized);

  MPI_Gather(
    local_C, M * np, MPI_INT, C, 1, col_c_resized, 0, MPI_COMM_WORLD
  );

  if (rank == 0) {
    printf("Rezultujuca matrica C:\n");
    for (int i = 0; i < M; i++) {
      for (int j = 0; j < N; j++)
        printf("%d\t", C[i][j]);
      printf("\n");
    }
  }

  MPI_Type_free(&col_c_type);
  MPI_Type_free(&col_c_resized);

  MPI_Finalize();
  return 0;
}
