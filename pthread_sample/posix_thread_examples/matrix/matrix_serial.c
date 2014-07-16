/********************************************************
 * An example source module to accompany...
 *
 * "Using POSIX Threads: Programming with Pthreads"
 *     by Brad nichols, Dick Buttlar, Jackie Farrell
 *     O'Reilly & Associates, Inc.
 *
 ********************************************************
 * matrix_serial.c
 *
 * A simple matrix multiplication program
 * (Matrix_A  X  Matrix_B) => Matrix_C
 */

#include <stdio.h>
#define ARRAY_SIZE 10

typedef int matrix_t[ARRAY_SIZE][ARRAY_SIZE];

matrix_t MA,MB,MC;


/*
* Routine to multiply a row by a column and place element in 
* resulting matrix.
*/
void mult(int size,
	  int row,
	  int column,
	  matrix_t MA, 
	  matrix_t MB,
	  matrix_t MC)
{
  int position;

  MC[row][column] = 0;
  for(position = 0; position < size; position++) {
    MC[row][column] = MC[row][column] +
      ( MA[row][position]  *  MB[position][column] ) ;
  }
}


/*
 * Main: allocates matrix, assigns values, computes the results
 */
extern int 
main(void)
{
  int      size, row, column;


  /* Currently size hardwired to ARRAY_SIZE size */
  size = ARRAY_SIZE;
  
  /* Fill in matrix values, currently values are hardwired */
  for (row = 0; row < size; row++) {
    for (column = 0; column < size; column++) {
      MA[row][column] = 1;
    }
  }
  for (row = 0; row < size; row++) {
    for (column = 0; column < size; column++) {
      MB[row][column] = row + column + 1;
    }
  }
  printf("MATRIX: The A array is;\n");
  for(row = 0; row < size; row ++) {
    for (column = 0; column < size; column++) {
      printf("%5d ",MA[row][column]);
    }
    printf("\n");
  }
  printf("MATRIX: The B array is is;\n");
  for(row = 0; row < size; row ++) {
    for (column = 0; column < size; column++) {
      printf("%5d ",MB[row][column]);
    }
    printf("\n");
  }

  
  /* Process Matrix, by row, column. */
  for(row = 0; row < size; row++) {
    for (column = 0; column < size; column++) {
      mult(size, row, column, MA, MB, MC);
    }
  }

  /* Print results */
  printf("MATRIX: The resulting matrix C is;\n");
  for(row = 0; row < size; row ++) {
    for (column = 0; column < size; column++) {
      printf("%5d ",MC[row][column]);
    }
    printf("\n");
  }
  return 0;
}




