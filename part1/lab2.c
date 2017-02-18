#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <sys/time.h>

#define SIZE_MAX 64
#define THREADS_MAX 401

pthread_mutex_t lock;
pthread_t threads[SIZE_MAX][SIZE_MAX];

typedef struct matrix_t {
  int matrix_a[SIZE_MAX][SIZE_MAX];
  int matrix_b[SIZE_MAX][SIZE_MAX];
  int matrix_c[SIZE_MAX][SIZE_MAX];
  int a_rows, a_columns;
  int b_rows, b_columns;
  int c_rows, c_columns;
} matrix_t;

typedef struct matrix_row_t {
  matrix_t *mtx;
  int current_row;
} matrix_rows_t;

typedef struct matrix_elem_t {
  matrix_t *mtx;
  int current_row;
  int current_col;
} matrix_elem_t;

// prototypes
void validate_input(int argc, char **argv);
int open_input_file(char *filename, matrix_t *);
void matrix_multiply_single(void *);
void matrix_multiply_rows(void *);
void matrix_multiply_elements(void *);
void matrix_add_single(void *);
void matrix_add_rows(void *);
void matrix_add_elements(void *);
void print_matrix_c(matrix_t *m);

void print_matrix_c(matrix_t *mtx) {
  if(NULL == mtx)
    {
      return;
    }
  int i,j;
  for(i=1; i<=mtx->c_rows; i++)
    {
      // Column
      for(j=1; j<=mtx->c_columns; j++)
	{
	  printf("%d ", mtx->matrix_c[i][j]);
	}
      printf("\n");
    }
}

void matrix_add_elements(void *arg) {
  if(NULL == arg)
    {
      return;
    }
  
  matrix_elem_t *matrix_elems = (matrix_elem_t *)arg;
  if(NULL == matrix_elems->mtx)
    {
      return;
    }
  matrix_t *mtx = matrix_elems->mtx;
  int i = matrix_elems->current_row;
  int j = matrix_elems->current_col;
  mtx->matrix_c[i][j] = mtx->matrix_a[i][j] + mtx->matrix_b[i][j];
}

int main(int argc, char **argv) {
  // To store the matricies
  matrix_t mtx;
  matrix_rows_t matrix_rows[SIZE_MAX];
  matrix_elem_t matrix_elements[SIZE_MAX][SIZE_MAX];
  char opt;

  // For measuring elapsed time
  struct timeval tv_before, tv_after;
  long int tv_difference_usec;

  // iterators
  int i,j,l;

  // Check input parameters
  validate_input(argc, argv);
  int ret = open_input_file(argv[1], &mtx);
  if(ret < 0)
    {
      printf("Error opening input file: %s\n", argv[1]);
      return -1;
    }

  opt = *argv[2];
  // Addition
  if('a' == opt || 'A' == opt)
    {
      // Check matrix dimensons
      if( (mtx.a_columns != mtx.b_columns) || (mtx.a_rows != mtx.b_rows) )
	{
	  printf("Invalid dimensions for addition\n");
	  return -1;
	}
      
      // Get start time
      gettimeofday(&tv_before, NULL);
      // Single thread addition
      pthread_create(&threads[0][0], NULL, (void *)&matrix_add_single, (void *)&mtx);
      pthread_join(threads[0][0], NULL);
      // Get end time
      gettimeofday(&tv_after, NULL);
      tv_difference_usec = tv_after.tv_usec - tv_before.tv_usec;
      // Print results of single thread addition
      printf("\nResults of addition with one thread: \n");
      print_matrix_c(&mtx);
      printf("Elapsed time with one thread: %ld microseconds\n", tv_difference_usec);

      // 1 thread per row
      // Get start time
      gettimeofday(&tv_before, NULL);
      for(i=1; i<=mtx.a_rows; i++)
	{
	  matrix_rows[i].mtx = &mtx;
	  matrix_rows[i].mtx->c_rows = mtx.a_rows;
	  matrix_rows[i].mtx->c_columns = mtx.a_columns;
	  matrix_rows[i].current_row = i;
	  pthread_create(&threads[i][0], NULL, (void *)&matrix_add_rows, (void *)&matrix_rows[i]);
	}
      //Wait for threads to finish
      for(i=1; i<=mtx.a_rows; i++)
	{
	  pthread_join(threads[i][0], NULL);
	}
      // Get end time
      gettimeofday(&tv_after, NULL);
      tv_difference_usec = tv_after.tv_usec - tv_before.tv_usec;
      printf("\nResults of addition with %d threads: \n", mtx.c_rows);
      print_matrix_c(&mtx);
      printf("Elapsed time with %d threads: %ld microseconds\n", mtx.c_rows, tv_difference_usec);

      // 1 thread per element
      // Get start time
      gettimeofday(&tv_before, NULL);
      for(i=1; i<=mtx.a_rows; i++)
	{
	  for(j=1; j<=mtx.a_columns; j++)
	    {
	      matrix_elements[i][j].mtx = &mtx;
	      matrix_elements[i][j].mtx->c_rows = mtx.a_rows;
	      matrix_elements[i][j].mtx->c_columns = mtx.a_columns;
	      matrix_elements[i][j].current_row = i;
	      matrix_elements[i][j].current_col = j;
	      pthread_create(&threads[i][j], NULL, (void *)&matrix_add_elements, (void *)&matrix_elements[i][j]);
	    }
	}
      for(i=1; i<=mtx.a_rows; i++)
	{
	  for(j=1; j<=mtx.a_columns; j++)
	    {
	      pthread_join(threads[i][j], NULL);
	    }
	}
      // Get end time
      gettimeofday(&tv_after, NULL);
      tv_difference_usec = tv_after.tv_usec - tv_before.tv_usec;
      printf("\nResults of addition with %d threads: \n", mtx.c_rows*mtx.c_columns);
      print_matrix_c(&mtx);
      printf("Elapsed time with %d threads: %ld microseconds\n", mtx.c_rows*mtx.c_columns, tv_difference_usec);
    }
    // Multiplication
  else if('m' == opt || 'M' == opt)
    {
      // Check dimensions for multiplication
      if(mtx.a_columns != mtx.b_rows)
	{
	  printf("Invalid dimensions for multiplication\n");
	  return -1;
	}
      // Assign dimensions of output matrix
      mtx.c_rows = mtx.a_rows;
      mtx.c_columns = mtx.b_columns;

      // Multiplication with one thread
      // Get start time
      gettimeofday(&tv_before, NULL);
      pthread_create(&threads[0][0], NULL, (void *)&matrix_multiply_single, (void *)&mtx);
      // Wait for thread to finish
      pthread_join(threads[0][0], NULL);
      // Get end time
      gettimeofday(&tv_after, NULL);
      tv_difference_usec = tv_after.tv_usec - tv_before.tv_usec;
      // Print results of single thread multiplication
      printf("\nResults of multiplication with one thread: \n");
      print_matrix_c(&mtx);
      printf("Elapsed time with one thread: %ld microseconds\n", tv_difference_usec);

      // One thread per row of output matrix
      // Get start time
      gettimeofday(&tv_before, NULL);
      //For each row in matrix A
      for(i=1; i<=mtx.a_rows; i++)
	{
	  matrix_rows[i].mtx = &mtx;
	  matrix_rows[i].current_row = i;
	  pthread_create(&threads[i][0], NULL, (void *)&matrix_multiply_rows, (void *)&matrix_rows[i]);
	}
      //Wait for threads to finish
      for(i=1; i<=mtx.a_rows; i++)
	{
	  pthread_join(threads[i][0], NULL);
	}
      // Get end time
      gettimeofday(&tv_after, NULL);
      tv_difference_usec = tv_after.tv_usec - tv_before.tv_usec;
      printf("\nResults of multiplication with %d threads: \n", mtx.c_rows);
      print_matrix_c(&mtx);
      printf("Elapsed time with %d threads: %ld microseconds\n", mtx.c_rows, tv_difference_usec);
    }

    // One thread per element of output matrix
    // Get start time
    gettimeofday(&tv_before, NULL);
    // For each row in Matrix A
    for(i=1; i<=mtx.a_rows; i++)
      {
	// For each column in matrix B
	for(l=1; l<=mtx.b_columns; l++)
	  {
	    matrix_elements[i][l].mtx = &mtx;
	    matrix_elements[i][l].current_row = i;
	    matrix_elements[i][l].current_col = l;
	    pthread_create(&threads[i][l], NULL, (void *)&matrix_multiply_elements, (void *)&matrix_elements[i][l]);
	  }
      }
    // wait for threads to finish
    for(i=1; i<=mtx.a_rows; i++)
      {
	// For each column in matrix B
	for(l=1; l<=mtx.b_columns; l++)
	  {
	    pthread_join(threads[i][l], NULL);
	  }
      }
      // Get end time
      gettimeofday(&tv_after, NULL);
      tv_difference_usec = tv_after.tv_usec - tv_before.tv_usec;
      printf("\nResults of multiplication with %d threads: \n", mtx.c_rows*mtx.c_columns);
      print_matrix_c(&mtx);
      printf("Elapsed time with %d threads: %ld microseconds\n", mtx.c_rows*mtx.c_columns, tv_difference_usec);


      return EXIT_SUCCESS;
}

void matrix_multiply_elements(void *args) {
  if(NULL == args)
    {
      return;
    }
  matrix_elem_t *matrix_elements = (matrix_elem_t *)args;
  if(NULL == matrix_elements->mtx)
    {
      return;
    }
  matrix_t *mtx = matrix_elements->mtx;
  int i = matrix_elements->current_row;
  int l = matrix_elements->current_col;
  int k;
  int sum_tmp = 0;
  for(k=1; k<=mtx->b_rows; k++)
    {
      sum_tmp += ( (mtx->matrix_a[i][k]) * (mtx->matrix_b[k][l]) );
    }
  mtx->matrix_c[i][l] = sum_tmp;
}

void matrix_multiply_rows(void *args) {
  if(NULL == args)
    {
      return;
    }
  matrix_rows_t *matrix_rows = (matrix_rows_t *)args;
  if(NULL == matrix_rows->mtx)
    {
      return;
    }
  matrix_t *mtx = (matrix_t *)matrix_rows->mtx;
  int i = matrix_rows->current_row;
  int sum_tmp=0;
  
  int l, k;
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


void matrix_multiply_single(void *m) {
  if(NULL == m)
    {
      return;
    }
  matrix_t *mtx = (matrix_t *)m;
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
  printf("\nMatrix A:\nNumber of rows: %d\nNumber of columns: %d\n", m->a_rows, m->a_columns);

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
	  //printf("(%d, %d): %d\n", i-2, j+1, m->matrix_a[i-2][j+1]);
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
  printf("\nMatrix B:\nNumber of rows: %d\nNumber of columns: %d\n", m->b_rows, m->b_columns);

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
	  //printf("(%d, %d): %d\n", i-1, j+1, m->matrix_b[i-1][j+1]);
	  tok = strtok(NULL, " \n");
	  j++;
	}
    }

  return 0;
}


