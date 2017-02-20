#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <rtai.h>
#include <rtai_lxrt.h>

#define BUF_SIZE 128
#define STACK_SIZE 512
#define MSG_SIZE 256

// Global buffer for threads
char buf[BUF_SIZE];
static RTIME timer_period;
static RT_TASK *rt1, *rt2, *rt3;

// struct to pass to read_input_file() 
typedef struct args {
  const char *filename; 
  const int thread_id;
} args_t;

// prototypes
void read_input_file(void *);
void read_from_buffer(void *);


int main(int argc, char **argv) {
  const args_t args1 = { "first.txt", 1 };
  const args_t args2 = { "second.txt", 2 };
  pthread_t t1, t2, t3;

  // 1ms timer period 
  // Only call start_rt_timer once
  timer_period = start_rt_timer(nano2count(10000000));

  // Create threads
  pthread_create(&t1, NULL, (void *)read_input_file, (void *)&args1);
  pthread_create(&t2, NULL, (void *)read_input_file, (void *)&args2);
  pthread_create(&t3, NULL, (void *)read_from_buffer, NULL);

  // Wait for Threads 1 and 2 to complete. (Both reach EOF)
  pthread_join(t1, NULL);
  pthread_join(t2, NULL);
  // Thread 3 is infinite loop. It will terminate with the process.

  stop_rt_timer();
  return 0;
}


// Input params: args_t *args_st
// Returns: nothing
void read_input_file(void *args_st) {
  // Validate parameters
  if(NULL == args_st)
    {
      return;
    }
  args_t *args = (args_t *)args_st;
  if(NULL == args->filename)
    {
      return;
    }
  FILE *fp_r = fopen(args->filename, "r");
  if(NULL == fp_r)
    {
      return;
    }	

  // Schedule periodic real time task. 
  // Task period is relative to the rt_timer period (internal count unit/clock ticks) and is determined from args->thread_id
  RTIME offset, task_period;
  int ret;
  if(1 == args->thread_id)
    {
      // thread id 1 corresponds to task 1 and file 1
      rt1 = rt_task_init(nam2num("rt1"), 0, STACK_SIZE, MSG_SIZE);
      // Start task 1 immediately with no offset.
      // Task 1 needs to be rescheduled every 4 timer periods
      offset = 0; //*timer_period;
      task_period = (4*timer_period); 
      ret = rt_task_make_periodic(rt1, rt_get_time()+offset, task_period);
      if(ret < 0)
	{
	  return;
	}
    }
  else if(2 == args->thread_id)
    {
      // thread id 2 for task 2 and file 2
      rt2 = rt_task_init(nam2num("rt2"), 0, STACK_SIZE, MSG_SIZE);
      // Start task 2 after two timer periods
      // Then reschedule  every 4 timer periods
      offset = (2*timer_period);
      task_period = (4*timer_period);
      ret = rt_task_make_periodic(rt2, rt_get_time()+offset, task_period);
      if(ret < 0)
	{
	return;
      }
    }

    while( (fgets(buf, BUF_SIZE, fp_r) != NULL) )
    {
	// Put a line from the file in the buffer and wait to be scheduled again
	rt_task_wait_period();
    }
    fclose(fp_r);
}


void read_from_buffer(void *args) {
  // Task 3 / Thread 3
  rt3 = rt_task_init(nam2num("rt3"), 0, STACK_SIZE, MSG_SIZE);
  // Offset task 3 by 1 timer_period (1st schedule should be immediately after task_1 1st deadline)
  // Task 3 needs rescheduled every two timer periods.
  // Should be scheduled immediately after Task 1 or 2 completes, in alternating order.
  RTIME offset = (1*timer_period);
  RTIME task_period = (2*timer_period);
  int ret = rt_task_make_periodic(rt3, rt_get_time()+offset, task_period);
  if(ret < 0)
    {
      return;
    }

  // inf loop
  while(1)
    {
      // Print a line from the global buffer being filled by task1 and task2
      printf("%s\n", buf);
      // Wait until rescheduled.
      rt_task_wait_period();
    }
}
