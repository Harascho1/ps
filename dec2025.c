#include <stdio.h>
#include "mpi.h"
#include <string.h>

#define r 4
#define m 3
#define k 5


int main(int argc, char * argv[])
{
    int size, rank;
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    int A[r][m], B[m][k], C[r][k];
    if (rank == 0) {
        for (int i = 0; i < r; i++)
        {
            for (int j = 0; j < m; j++)
            {
                A[i][j] = i + j;
                printf("%d\t", A[i][j]);
            }
            printf("\n");   
        }
        for (int i = 0; i < m; i++)
        {
            for (int j = 0; j < k; j++)
            {
                B[i][j] = i + j;
                printf("%d\t", B[i][j]);
            }
            printf("\n");
        }
    }

    MPI_Datatype rowA;
    MPI_Type_contiguous(m, MPI_INT, &rowA);
    MPI_Type_commit(&rowA);

    int rows_per_proc = r / size;
    int local_A[rows_per_proc][m];
    int local_C[rows_per_proc][k];

    MPI_Bcast(&B[0][0], m*k, MPI_INT,  0, MPI_COMM_WORLD);
    MPI_Scatter(&A[0][0], rows_per_proc, rowA, &local_A[0][0], rows_per_proc, rowA, 0, MPI_COMM_WORLD);

    for( int i = 0; i < rows_per_proc; i++) {
        for(int j = 0; j < k ; j++){
            local_C[i][j] = 0;
            for(int p = 0; p < m; p++){
                local_C[i][j] += local_A[i][p] * B[p][j];
            }
        }
    }

    MPI_Gather(&local_C[0][0], rows_per_proc, rowA, &C[0][0], rows_per_proc, rowA, 0, MPI_COMM_WORLD);

    if (rank == 0) {
        printf("\n=== Rezultat C = A * B (%d x %d) ===\n", r, k);
        for (int i = 0; i < r; i++) {
            for (int j = 0; j < k; j++) {
                printf("%d\t", C[i][j]);
            }
            printf("\n");
        }
    }

    MPI_Type_free(&rowA);

    MPI_Finalize();
}