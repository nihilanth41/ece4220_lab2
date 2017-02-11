#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#define BUF_SIZE 128


typedef struct matrix_t {
  int matrix_a[BUF_SIZE][BUF_SIZE];
  int matrix_b[BUF_SIZE][BUF_SIZE];
  int matrix_add[BUF_SIZE][BUF_SIZE];
  int matrix_mul[BUF_SIZE][BUF_SIZE];
  int a_rows, a_columns;
  int b_rows, b_columns;
  int mul_rows, mul_columns;
  int add_rows, add_columns;
} matrix_t;

// prototypes
void validate_input(int argc, char **argv);
int open_input_file(char *filename, matrix_t *);
void matrix_multiply_single(void *);



int main(int argc, char **argv) {

  validate_input(argc, argv);

  matrix_t mtx;
  int ret = open_input_file(argv[1], &mtx);
  if(ret < 0)
    {
      printf("Error opening input file\n");
      return -1;
    }
  // Check matrix dimensons
  // Multiplication first
  if(mtx.a_columns != mtx.b_rows)
    {
      printf("Invalid dimensions for multiplication\n");
      return -1;
    }
  // Get start time
  // Create threads (do computation)
  // Single thread for entire output
  pthread_t thread_s;
  pthread_create(&thread_s, NULL, (void *)&matrix_multiply_single, (void *)&mtx);

  // Wait for thread to finish
  pthread_join(thread_s, NULL);

  // Get end time

  // print result matrix

  int i,j;
  // Row
  for(i=1; i<=mtx.mul_rows; i++)
    {
      // Column
      for(j=1; j<=mtx.mul_columns; j++)
	{
	  printf("%d ", mtx.matrix_mul[i][j]);
	}
      printf("\n");
    }

  // print time elapsed


  return 0;
}

void matrix_multiply_single(void *m) {
  if(NULL == m)
    {
	exit(-1);
    }
    matrix_t *mtx = (matrix_t *)m;
    mtx->mul_rows = mtx->a_rows;
    mtx->mul_columns = mtx->b_columns;
    int i,k,l;
    int sum_tmp=0;
    // For each row in Matrix A
    for(i=1; i<=mtx->a_rows; i++)
      {
	// For each column in matrix B
	for(l=1; l<=mtx->b_columns; l++)
	  {
	    // For each row in matrix B
	    for(k=1; k<=mtx->b_rows; k++)
	      {
		sum_tmp +=  ( (mtx->matrix_a[i][k]) * (mtx->matrix_b[k][l]) );
	      }
	    mtx->matrix_mul[i][l] = sum_tmp;
	    sum_tmp = 0;
	  }
      }

    pthread_exit(0);
}

void validate_input(int argc, char **argv) {
  // Get filename arg
  if(argc < 2)
    {
      printf("Usage is: %s </path/to/matrix.txt>\n", argv[0]);
      exit(-1);
    }
  FILE *fp_r = fopen(argv[1], "r");
  if(NULL == fp_r) 
    {
      printf("Error opening file: %s\n", argv[1]);
      exit(-1);
    }
}

int open_input_file(char *filename, matrix_t *m) {
  FILE *fp_r = fopen(filename, "r");
  if(NULL == fp_r) 
    {
      return -1;
    }
  // Get row/column size of matrix A from file 
  fscanf(fp_r, "%d %d", &(m->a_rows), &(m->a_columns));
  printf("Matrix A:\nNumber of rows: %d\nNumber of columns: %d\n", m->a_rows, m->a_columns);

  // Read matrix A from file	
  char buf[BUF_SIZE];
  int i=1;
  // Add 2 to rows because the first row doesn't count
  // and 0-index
  while( (fgets(buf, 128, fp_r) != NULL) && (i<(m->a_rows)+2) )
    {
      // Increment line count
      i++;
      // Tokenize the line into numbers
      char *tok = strtok(buf, " \n");
      // Keep track of column while tokenizing
      int j=0;
      while( tok != NULL )
	{
	  m->matrix_a[i-2][j+1] = atoi(tok);
	  //printf("(%d, %d): %s\n", i-2, j+1, tok);
	  printf("(%d, %d): %d\n", i-2, j+1, m->matrix_a[i-2][j+1]);
	  tok = strtok(NULL, " \n");
	  j++;
	}
    }
	
  // Matrix B row/column size is in buf at this point
  char *tok = strtok(buf, " \n");
  if(tok == NULL)
    {
      return -1;
    }
  m->b_rows = atoi(tok);
  m->b_columns = atoi(strtok(NULL, " \n"));
  printf("Matrix B:\nNumber of rows: %d\nNumber of columns: %d\n", m->b_rows, m->b_columns);
	
  // Read matrix B from file	
  i=1;
  // Add 2 to rows because the first row doesn't count
  // and 0-index
  while( (fgets(buf, 128, fp_r) != NULL) && (i<(m->b_rows)+1) )
    {
      // Increment line count
      i++;
      // Tokenize the line into numbers
      char *tok = strtok(buf, " \n");
      // Keep track of column while tokenizing
      int j=0;
      while( tok != NULL )
	{
	  m->matrix_b[i-1][j+1] = atoi(tok);
	  //printf("(%d, %d): %s\n", i-1, j+1, tok);
	  printf("(%d, %d): %d\n", i-1, j+1, m->matrix_b[i-1][j+1]);
	  tok = strtok(NULL, " \n");
	  j++;
	}
    }

  return 0;
}

 
