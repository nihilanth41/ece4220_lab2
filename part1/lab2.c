#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#define SIZE_MAX 64 

pthread_mutex_t lock;
pthread_t threads[SIZE_MAX];

typedef struct matrix_t {
  int matrix_a[SIZE_MAX][SIZE_MAX];
  int matrix_b[SIZE_MAX][SIZE_MAX];
  int matrix_c[SIZE_MAX][SIZE_MAX];
  int a_rows, a_columns;
  int b_rows, b_columns;
  int c_rows, c_columns;
} matrix_t;

typedef struct matrix_rows_t {
  matrix_t *mtx;
  int current_row;
} matrix_rows_t;

// prototypes
void validate_input(int argc, char **argv);
int open_input_file(char *filename, matrix_t *);
void matrix_multiply_single(void *);
void matrix_add_single(void *);
void matrix_add_rows(void *);
void matrix_add_rows_wrapper(matrix_rows_t *);

void matrix_add_rows(void *m) {
  matrix_rows_t *matrix_rows = (matrix_rows_t *)m;
  matrix_t *mtx = matrix_rows->mtx;
  int i = matrix_rows->current_row;
  int j;
  for(j=1; j<=mtx->a_columns; j++)
    {
      mtx->matrix_c[i][j] = mtx->matrix_a[i][j] + mtx->matrix_b[i][j];
    }
}

void matrix_add_rows_wrapper(matrix_rows_t *m) {
  // For each row of the output matrix, spin up a new thread to do the work
  int i;
  for(i=0; i<m->mtx->a_rows; i++)
    {
      m->current_row = i+1;
      pthread_create(&threads[i], NULL, (void *)&matrix_add_rows, (void *)m);
    }

  // Wait for threads to finish
  for(i=0; i<m->mtx->a_rows; i++)
    {
      pthread_join(threads[i], NULL);
    }

  m->mtx->c_rows = m->mtx->a_rows;
  m->mtx->c_columns = m->mtx->a_columns;
}
  
int main(int argc, char **argv) {
 // To store the matricies
  matrix_t mtx;
  char opt;

  // Check input parameters
  validate_input(argc, argv);

  int ret = open_input_file(argv[1], &mtx);
  if(ret < 0)
    {
      printf("Error opening input file: %s\n", argv[1]);
      return -1;
    }
  
  opt = *argv[2];
  if('a' == opt || 'A' == opt)
    {
      // Check matrix dimensons
      if( (mtx.a_columns != mtx.b_columns) || (mtx.a_rows != mtx.b_rows) )
	{
	  printf("Invalid dimensions for addition\n");
	  return -1;
	}
      // Addition
      // Get start time
      //pthread_create(&threads[1], NULL, (void *)&matrix_add_single, (void *)&mtx);
      // Wait for thread to finish
      //pthread_join(threads[1], NULL);
      // 1 thread per row
      matrix_rows_t matrix_rows;
      matrix_rows.mtx = &mtx;
      matrix_add_rows_wrapper(&matrix_rows);
      // Get end time
    }
  else if('m' == opt || 'M' == opt)
    {

      if(mtx.a_columns != mtx.b_rows)
	{
	  printf("Invalid dimensions for multiplication\n");
	  return -1;
	}
      // Multiplication
      // Get start time
      pthread_create(&threads[1], NULL, (void *)&matrix_multiply_single, (void *)&mtx);
      // Wait for thread to finish
      pthread_join(threads[1], NULL);
      // Get end time
    }
	

  // print result matrix
  int i,j;
  for(i=1; i<=mtx.c_rows; i++)
    {
      // Column
      for(j=1; j<=mtx.c_columns; j++)
	{
	  printf("%d ", mtx.matrix_c[i][j]);
	}
      printf("\n");
    }

  return 0;
}

void matrix_add_single(void *m) {
  if(NULL == m)
    {
      exit(-1);
    }
  matrix_t *mtx = (matrix_t *)m;
  mtx->c_rows = mtx->a_rows;
  mtx->c_columns = mtx->a_columns;
  int i,j;
  // For each row
  for(i=1; i<=mtx->c_rows; i++)
    {
      // For each column
      for(j=1; j<=mtx->c_columns; j++)
	{
	  mtx->matrix_c[i][j] = mtx->matrix_a[i][j] + mtx->matrix_b[i][j];
	}
    }
  pthread_exit(0);
}

void matrix_multiply_single(void *m) {
  if(NULL == m)
    {
      exit(-1);
    }
  matrix_t *mtx = (matrix_t *)m;
  mtx->c_rows = mtx->a_rows;
  mtx->c_columns = mtx->b_columns;
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
	  mtx->matrix_c[i][l] = sum_tmp;
	  sum_tmp = 0;
	}
    }

  pthread_exit(0);
}

void validate_input(int argc, char **argv) {
  // Get filename arg
  if(argc < 3)
    {
      printf("Usage is: %s </path/to/file.txt> <A(dd)|M(ultiply)>", argv[0]);
      exit(-1);
    }
  FILE *fp_r = fopen(argv[1], "r");
  if(NULL == fp_r) 
    {
      printf("Error opening file: %s\n", argv[1]);
      exit(-1);
    }
  if(*argv[2] != 'A' && *argv[2] != 'a' && *argv[2] != 'M' && *argv[2] != 'm')
    {
      printf("Usage is: %s </path/to/file.txt> <A(dd)|M(ultiply)>", argv[0]);
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
  char buf[SIZE_MAX];
  int i=1;
  // Add 2 to rows because the first row doesn't count
  // and 0-index
  while( (fgets(buf, SIZE_MAX, fp_r) != NULL) && (i<(m->a_rows)+2) )
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
  while( (fgets(buf, SIZE_MAX, fp_r) != NULL) && (i<(m->b_rows)+1) )
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

 
