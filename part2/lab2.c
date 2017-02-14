#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <rtai.h>
#include <rtai_lxrt.h>

#define SIZE_MAX 128
#define STACK_SIZE 512
#define MSG_SIZE 256

// Global buffer for threads
char buf[SIZE_MAX];
static RTIME period;
static RT_TASK *rt1, *rt2, *rt3;

// input char *filename - path to file to open
// return: 0 on success
// -1 on error

typedef struct args {
	char *filename; 
	int thread_id;
} args_t;

void read_input_file(void *file) {
	if(NULL == file)
	{
		return -1;
	}
	args_t *args = (args_t *)file;
	char *filename = args->filename;
	if(NULL == filename)
	{
		return -1;
	}
	FILE *fp_r = fopen(filename, "r");
	if(NULL == fp_r)
	{
		return -1;
	}	
	
	int thread_id = args->thread_id
	RTIME offset;
	if(0 == thread_id)
	{
		// Init real time task
		rt1 = rt_task_init(nam2num("rt1"), 0, STACK_SIZE, MSG_SIZE);
		offset = 0;
		rt_task_make_periodic(rt1, rt_get_time()+(offset*period), period);
	}
	else
	{
		rt2 = rt_task_init(nam2num("rt2"), 0, STACK_SIZE, MSG_SIZE);
		offset = 2;
		rt_task_make_periodic(rt2, rt_get_time()+(offset*period), period);
	}
		
	// Make thread periodic
	while( (fgets(buf, SIZE_MAX, fp_r) != NULL) )
	{
		// Line in buf
		// Wait for the next period
		rt_task_wait_period();
	}
	fclose(fp_r);
}

void read_from_buffer(void *) {
	rt3 = rt_task_init(nam2num("rt3"), 0, STACK_SIZE, MSG_SIZE);
	RTIME offset = 1;
	rt_task_make_periodic(rt3, rt_get_time()+(offset*period), period);
	printf("%s\n", buf);
}

int main(int argc, char **argv) {
	// Create 3 threads and a common buffer
	// First thread opens first.txt 
	// Second thread opens second.txt

	const char *file1 = "first.txt";
	const char *file2 = "second.txt";

	pthread_t t1, t2, t3;
	// Only call once
	// 1mS
	period = start_rt_timer(nano2count(1000000));
	pthread_create(&t1, NULL, (void *)read_input_file, (void *)file1);
	pthread_create(&t2, NULL, (void *)read_input_file, (void *)file2);
	pthread_create(&t3, NULL, (void *)read_from_buffer, NULL);

	pthread_join(t1, NULL);
	pthread_join(t2, NULL);
	// t3 is infinite loop

	stop_rt_timer();
	return 0;
}

