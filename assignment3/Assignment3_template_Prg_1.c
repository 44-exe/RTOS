/**********************************************************************************

       *************NOTE**************
This is a template for the subject of RTOS in University of Technology Sydney(UTS)
Please complete the code based on the assignment requirement.

Assignment 3 Program_2 template

**********************************************************************************/
/*
  To compile prog_1 ensure that gcc is installed and run the following command:
  gcc -Wall -O2 program_1.c -o prog_1 -lpthread -lrt

*/

#include <pthread.h>   /* pthread functions and data structures for pipe */
#include <unistd.h>   /* for POSIX API */
#include <stdlib.h>   /* for exit() function */
#include <stdio.h>  /* standard I/O routines */
#include <stdbool.h>
#include <string.h>
#include <semaphore.h>
#include <time.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/shm.h>
#include <sys/stat.h>

#define MAX_PROCESS 7

/****** STRUCTURES ******/
typedef struct SRTF_Params {
  //add your variables here
  int pid;//process id
  int arrive_t, wait_t, burst_t, turnaround_t, remain_t;//process time
} Process_Params;

/****** FUNCTIONS ******/
void input_processes(void);
void process_SRTF(void);
void calculate_average(void);
void print_results(void);
void send_FIFO(void);
void read_FIFO(void);


//Array of processes with 1 extra for placeholder remain_t
Process_Params processes[MAX_PROCESS + 1];
//Index global variable
int i;
//Averages calculated
float avg_wait_t = 0.0, avg_turnaround_t = 0.0;
//Semaphore
sem_t sem_SRTF;
//Pthreads
pthread_t thread1, thread2;

FILE * pfile_out;

/* this function calculates CPU SRTF scheduling, writes waiting time and turn-around time to the FIFO */
void *worker1(void *params)
{
  // add your code here
  input_processes();
  process_SRTF();
  calculate_average();
  send_FIFO();
  return NULL;
}

/* reads the waiting time and turn-around time through the FIFO and writes to text file */
void *worker2()
{
  // add your code here
  sem_wait(&sem_SRTF);
  print_results();
  read_FIFO();
  return NULL;
}


//Create process arrive times and burst times, taken from assignment details
void input_processes(void)
{
  processes[0].pid = 1; processes[0].arrive_t = 8;  processes[0].burst_t = 10;
  processes[1].pid = 2; processes[1].arrive_t = 10; processes[1].burst_t = 3;
  processes[2].pid = 3; processes[2].arrive_t = 14; processes[2].burst_t = 7;
  processes[3].pid = 4; processes[3].arrive_t = 9;  processes[3].burst_t = 5;
  processes[4].pid = 5; processes[4].arrive_t = 16; processes[4].burst_t = 4;
  processes[5].pid = 6; processes[5].arrive_t = 21; processes[5].burst_t = 6;
  processes[6].pid = 7; processes[6].arrive_t = 26; processes[6].burst_t = 2;
  
  //Initialise remaining time to be same as burst time
  for (i = 0; i < MAX_PROCESS; i++) {
  processes[i].remain_t = processes[i].burst_t;
  }
}


//Schedule processes according to SRTF rule
void process_SRTF(void)
{
  int endTime, smallest, time, remain = 0;

  //Placeholder remaining time to be replaced
  processes[MAX_PROCESS].remain_t=9999;

  //Run function until remain is equal to number of processes
  for (time = 0; remain != MAX_PROCESS; time++)
  {
    //Assign placeholder remaining time 'processes[8].remain_t=9999' as the smallest
    smallest = MAX_PROCESS;
  
    //Check all processes that have arrived for lowest remain time then set the lowest to be the smallest
    for (i=0;i<MAX_PROCESS;i++)
  {
      if (processes[i].arrive_t <= time && processes[i].remain_t < processes[smallest].remain_t && processes[i].remain_t > 0)
    {
        smallest = i;
      }
    } //end of 'for (i=0;i<PROCESSNUM;i++)'
  
    //Decrease remaining time as time increases
    processes[smallest].remain_t--;
  
    //If process is finished, save time information, add to average totals and increase remain
    if (processes[smallest].remain_t == 0)
  {
      remain++;//one process has been finished!
    
      endTime=time+1; //time must increase 1 => endTime
    
      processes[smallest].turnaround_t = endTime-processes[smallest].arrive_t;
      
    //endTime - (arrive+busrt)
      processes[smallest].wait_t = endTime-processes[smallest].burst_t-processes[smallest].arrive_t;
      
      avg_wait_t += processes[smallest].wait_t; // intermedia value
      
      avg_turnaround_t += processes[smallest].turnaround_t;//intermedia vaule
    } //end of 'processes[smallest].remain_t == 0'
  } //end of 'for (time = 0; remain != PROCESSNUM; time++)'
  
} // end of 'process_SRTF()'


//Simple calculate average wait time and turnaround time function
void calculate_average(void)
{
  //divided by PROCESSNUM = 8
  avg_wait_t /= MAX_PROCESS;
  avg_turnaround_t /= MAX_PROCESS;
}


// prints all result
void print_results(void)
{
  
  printf("Process Schedule Table: \n");
  
  printf("\tProcess ID\tArrival Time\tBurst Time\tWait Time\tTurnaround Time\n");
  
  for (i = 0; i<MAX_PROCESS; i++)
  {
    printf("\t%d\t\t%d\t\t%d\t\t%d\t\t%d\n", processes[i].pid,processes[i].arrive_t, 
            processes[i].burst_t, processes[i].wait_t, processes[i].turnaround_t);
  }
  
  printf("\nAverage wait time: %fs\n", avg_wait_t);
  
  printf("\nAverage turnaround time: %fs\n", avg_turnaround_t);
}


//Send and write average wait time and turnaround time to fifo
void send_FIFO(void)
{
  int res, fifofd;
  
  char * myfifo = "/tmp/myfifo1";
  
  res = mkfifo(myfifo, 0777);
  
  if (res < 0) {
    printf("mkfifo error\n");
    exit(0);
  }
  
  sem_post(&sem_SRTF);
  
  fifofd = open(myfifo, O_WRONLY);
  
  if (fifofd < 0) {
    printf("fifo open send error\n");
    exit(0);
  }
  
  write(fifofd, &avg_wait_t, sizeof(avg_wait_t));
  write(fifofd, &avg_turnaround_t, sizeof(avg_turnaround_t));
  
  close(fifofd);
  
  unlink(myfifo);
}

//Read average wait time and turnaround time from fifo then write to output.txt
void read_FIFO(void)
{
  int fifofd;
  
  float fifo_avg_turnaround_t, fifo_avg_wait_t;
  
  char * myfifo = "/tmp/myfifo1";
  
  fifofd = open(myfifo, O_RDONLY);
  
  if (fifofd < 0) {
    printf("fifo open read error\n");
    exit(0);
  }
  
  read(fifofd, &fifo_avg_wait_t, sizeof(int));
  read(fifofd, &fifo_avg_turnaround_t, sizeof(int));
  
  printf("\nRead from FIFO: %fs Average wait time\n", fifo_avg_wait_t);
  printf("\nRead from FIFO: %fs Average turnaround time\n", fifo_avg_turnaround_t);
  
  fprintf(pfile_out, "Average wait time: %f\n", fifo_avg_wait_t);
  fprintf(pfile_out, "Average turnaround time: %f\n", fifo_avg_turnaround_t);
  
  fclose(pfile_out);

  close(fifofd);
  
  remove(myfifo);
}



/* this main function creates named pipe and threads */
int main(int argc, char const *argv[])
{
  /* arguments from commandline */
  if(argc < 2)
  {
    fprintf(stderr,"usage: output.txt <file name> (for example: ./Assignment3_Prg1 output.txt)\n");
    fprintf(stderr,"%s \n", *argv);
    return -11;
  }

  pfile_out = fopen((char*)argv[1], "w");

  if (pfile_out == NULL)
  {
    printf("ERROR A: FAILED TO CREATE FILE!\n");
    return -12;
  }


  /* creating a named pipe(FIFO) with read/write permission */
  // add your code

  /* initialize the parameters */
  // add your code
  if(sem_init(&sem_SRTF, 0, 0)!=0)
  {
    printf("semaphore initialize erro \n");
    return -10;
  }
  
  /* create threads */
  // add your code
  if(pthread_create(&thread1, NULL, (void *)worker1, NULL)!=0)
  {
    printf("Thread 1 created error\n");
    return -1;
  }
  if(pthread_create(&thread2, NULL, (void *)worker2, NULL)!=0)
  {
    printf("Thread 2 created error\n");
    return -2;
  }

  if(pthread_join(thread1, NULL)!=0)
  {
    printf("join thread 1 error\n");
    return -3;
  }
  if(pthread_join(thread2, NULL)!=0)
  {
    printf("join thread 2 error\n");
    return -4;
  }
  
  /* wait for the thread to exit */
  //add your code
  if(sem_destroy(&sem_SRTF)!=0)
  {
    printf("Semaphore destroy error\n");
    return -5;
  }
  
  return 0;
}


// End of File
