#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>


int main(int argc, char **argv) {
	// Get filename arg
	if(argc < 2)
	{
		printf("Usage is: %s </path/to/matrix.txt>\n", argv[0]);
		return -1;
	}
	FILE *fp_r = fopen(argv[1], "r");
	if(NULL == fp_r) 
	{
		printf("Error opening file: %s\n", argv[1]);
		return -1;
	}
	#define SIZE_MAX 20		
	int rows_a, rows_b, columns_a, columns_b;
	int matrix_a[SIZE_MAX][SIZE_MAX];
	int matrix_b[SIZE_MAX][SIZE_MAX];
	// Get row/column size of matrix A from file 
	fscanf(fp_r, "%d %d", &rows_a, &columns_a);
	printf("Matrix A:\nNumber of rows: %d\nNumber of columns: %d\n", rows_a, columns_a);


	// Read matrix A from file	
	char buf[128];
	int i=1;
	// Add 2 to rows because the first row doesn't count
	// and 0-index
	while( (fgets(buf, 128, fp_r) != NULL) && (i<rows_a+2) )
	{
		// Increment line count
		i++;
		// Tokenize the line into numbers
		char *tok = strtok(buf, " \n");
		// Keep track of column while tokenizing
		int j=0;
		while( tok != NULL )
		{
			printf("(%d, %d): %s\n", i-2, j+1, tok);
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
	rows_b = atoi(tok);
	columns_b = atoi(strtok(NULL, " \n"));
	printf("Matrix B:\nNumber of rows: %d\nNumber of columns: %d\n", rows_b, columns_b);
	
	// Read matrix B from file	
	i=1;
	// Add 2 to rows because the first row doesn't count
	// and 0-index
	while( (fgets(buf, 128, fp_r) != NULL) && (i<rows_b+1) )
	{
		// Increment line count
		i++;
		// Tokenize the line into numbers
		char *tok = strtok(buf, " \n");
		// Keep track of column while tokenizing
		int j=0;
		while( tok != NULL )
		{
			printf("(%d, %d): %s\n", i-1, j+1, tok);
			tok = strtok(NULL, " \n");
			j++;
		}
	}

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
