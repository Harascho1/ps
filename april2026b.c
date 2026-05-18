#include "mpi.h"

#include <math.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct {
  char mat_br[13];
  char ime[30];
  char prezime[30];
  double avg_plata;
} ZAPOSLENI;

int main(int argc, char *argv[]) {
  int size, rank;

  // NOTE: inicijalizacija
  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD, &size);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  int side = (int)sqrt((double)size);
  int row = rank / side;
  int col = rank % side;

  /*
   * NOTE: pravimo donju matricu po pravilu (i < j)
   * procesi koji zadovoljaju pravilo imace flag = 1, a
   * oni koji ne flag = MPI_UNDEFINED
   */
  int color = (row > col) ? 1 : MPI_UNDEFINED;
  MPI_Comm donji_trougao_comm;
  MPI_Comm_split(MPI_COMM_WORLD, color, rank, &donji_trougao_comm);

  MPI_Datatype dt_zaposleni;
  int blocklenght[4] = {13, 30, 30, 1};
  MPI_Datatype tipovi[4] = {MPI_CHAR, MPI_CHAR, MPI_CHAR, MPI_DOUBLE};
  MPI_Aint displacemnets[4];

  /*
   * NOTE: stavljamo offset za svaki 'prop' koja struktura ima
   * Redosled mora da se poklapa sa redosledom blocklenght-a i niza 'tipovi'
   */
  displacemnets[0] = offsetof(ZAPOSLENI, mat_br);
  displacemnets[1] = offsetof(ZAPOSLENI, ime);
  displacemnets[2] = offsetof(ZAPOSLENI, prezime);
  displacemnets[3] = offsetof(ZAPOSLENI, avg_plata);

  MPI_Type_create_struct(4, blocklenght, displacemnets, tipovi, &dt_zaposleni);
  MPI_Type_commit(&dt_zaposleni);

  // NOTE: ovaj deo koda ce izvrisiti samo procesi koji su u donjem trouglu
  if (donji_trougao_comm != MPI_COMM_NULL) {
    int new_rank, new_size;
    MPI_Comm_rank(donji_trougao_comm, &new_rank);
    MPI_Comm_size(donji_trougao_comm, &new_size);

    int elementi_po_procesu = 2;
    ZAPOSLENI *svi_zaposleni = NULL;
    ZAPOSLENI *moji_zaposleni =
        (ZAPOSLENI *)malloc(elementi_po_procesu * sizeof(ZAPOSLENI));

    if (new_rank == 0) {
      printf("new size: %d", new_size);
      int total_elements = new_size * elementi_po_procesu;
      svi_zaposleni = (ZAPOSLENI *)malloc(total_elements * sizeof(ZAPOSLENI));
    }

    MPI_Scatter(svi_zaposleni, elementi_po_procesu, dt_zaposleni,
                moji_zaposleni, elementi_po_procesu, dt_zaposleni, 0,
                donji_trougao_comm);

    printf("Proces: globalni rank %d (red: %d, kolona: %d) | lokalni rank %d "
           "uspesno primio podatke.\n",
           rank, row, col, new_rank);

    if (new_rank == 0) {
      free(svi_zaposleni);
    }
    free(moji_zaposleni);
    MPI_Comm_free(&donji_trougao_comm);
  }
  MPI_Finalize();
}
