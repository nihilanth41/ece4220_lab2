#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <rtai.h>
#include <rtai_lxrt.h>

#define SIZE_MAX 128

int main(int argc, char **argv) {

	char buf[SIZE_MAX];
	RT_TASK t1, t2, t3;

	// Create 3 threads and a common buffer
	// First thread opens first.txt 
	// Second thread opens second.txt


	return 0;
}

