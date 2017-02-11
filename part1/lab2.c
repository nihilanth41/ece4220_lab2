#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#define BUF_SIZE 128
typedef struct matrix_t {
  int matrix_a[BUF_SIZE][BUF_SIZE];
  int matrix_b[BUF_SIZE][BUF_SIZE];
  int matrix_c[BUF_SIZE][BUF_SIZE];
  int a_rows, a_columns;
  int b_rows, b_columns;
  int c_rows, c_columns;
} matrix_t;

// prototypes
void validate_input(int argc, char **argv);
int open_input_file(char *filename, matrix_t *);

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
  



int main(int argc, char **argv) {

  validate_input(argc, argv);

  matrix_t matricies;
  int ret = open_input_file(argv[1], &matricies);
  if(ret < 0) { printf("Error opening input file\n"); return -1; }
  
  

	// Read matrix b from file	
	// Check matrix dimensons

	// Get start time

	// Create threads (do computation)

	// join threads

	// Get end time

	// print result matrix
	// print time elapsed


	return 0;
}
