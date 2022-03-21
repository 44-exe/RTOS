/***********************************************************************************/
//***********************************************************************************
//            *************NOTE**************
// This is a template for the subject of RTOS in University of Technology Sydney(UTS)
// Please complete the code based on the assignment requirement.

//***********************************************************************************
/***********************************************************************************/

/*
  To compile prog_1 ensure that gcc is installed and run the following command:
  gcc prog_1.c -o prog_1 -lpthread -lrt -Wall

*/
#include  <pthread.h>
#include  <stdlib.h>
#include  <unistd.h>
#include  <stdio.h>
#include  <sys/types.h>
#include  <fcntl.h>
#include  <string.h>
#include  <sys/stat.h>
#include  <semaphore.h>
#include  <sys/time.h>

#define MAX_BUFFER 255

/* --- Structs --- */

typedef struct ThreadParams {
  int pipeFile[2];
  sem_t sem_A, sem_B, sem_C;
  char message[MAX_BUFFER];
  pthread_mutex_t lock;
} ThreadParams;

pthread_attr_t attr;


int counter; /* buffer counter This is the global variable might cause critical section*/
int Max_counter; /* hold the Max counter unmber for aligned printing as input*/

int fd[2];//File descriptor for creating a pipe

/* --- Prototypes --- */
int insert_item(ThreadParams *p, u_int8_t item);
int remove_item(ThreadParams *p, u_int8_t *item);


/* Initializes data and utilities used in thread params */
void initializeData(ThreadParams *params);

/* This thread reads data from data.txt and writes each line to a pipe */
void* ThreadA(void *params);

/* This thread reads data from pipe used in ThreadA and writes it to a shared variable */
void* ThreadB(void *params);

/* This thread reads from shared variable and outputs non-header text to src.txt */
void* ThreadC(void *params);

/* --- Main Code --- */
int main(int argc, char const *argv[])
{
  int err, result;
  pthread_t tid[3];
  //pthread_attr_t attr;
  ThreadParams params;
  
  if (argc != 2) 
  {
    fprintf(stderr, "USAGE:./main.out data.txt\n");
  }
    
  // Initialization
  initializeData(&params);
  
  printf("DEBUG MAIN: Creating a pipe\n");
  result = pipe(fd);
  if (result < 0)
  {
      perror("pipe error");
      exit(1);
  }

  // Create Threads
  // pthread_create(&(tid[0]), &attr, &ThreadA, (void*)(&params));  // resource is not sufficient (may require change)
  if(pthread_create(&(tid[0]), &attr, &ThreadA, (void*)(&params)) !=0) 
  { 
    perror ("ERROR MAIN: failed create thread A\n"); 
    exit (1);
  }

  if(pthread_create(&(tid[1]), &attr, &ThreadB, (void*)(&params)) !=0) 
  { 
    perror ("ERROR MAIN: failed create thread B\n"); 
    exit (1);
  }

  if(pthread_create(&(tid[2]), &attr, &ThreadC, (void*)(&params)) !=0) 
  { 
    perror ("ERROR MAIN: failed create thread C\n"); 
    exit (1);
  }

  //TODO: add your code

  // Wait on threads to finish
  pthread_join(tid[0], NULL);  // Thread A
  pthread_join(tid[1], NULL);  // Thread B
  pthread_join(tid[2], NULL);  // Thread C

  
  //TODO: add your code

  return 0;
}

void initializeData(ThreadParams *params) {
  // Initialize Sempahores
  sem_init(&(params->sem_A), 0, 1);
  sem_init(&(params->sem_B), 0, 0);
  sem_init(&(params->sem_C), 0, 0);
  // Initialize thread attributes 
  pthread_attr_init(&attr);
  //TODO: add your code

  // Initialise mutex lock
  pthread_mutex_init(&(params->lock), NULL);

  counter = 0; 
  Max_counter = 0;
  return;
}

void *ThreadA(void *params)
{
  //TODO: add your code
  ThreadParams *p = params;
  // sem_wait(&(p->sem_A));
  printf("/****** RUNNING THREAD A ******/\n");

  printf("\nthread A read from data.txt\n");
  FILE *pfile;
  char c[255];
  char file_name[50] = "data.txt";
  char check[12] = "end_header\n";
  char eof[4] = "EOF";  // describes the end of file
  // int sig = 0;
  int result;
  
  if (file_name == NULL)
  {
    printf("ERROR A: invalid file name\n");
    exit(1);
  }
  pfile = fopen(file_name, "r");
  if (pfile == NULL)
  {
    printf("ERROR A: FAILED TO OPEN FILE!\n");
    exit(1);
  }

  // printf("DEBUG A: reading from the file\n");
  while(1)
  {
    sem_wait(&(p->sem_A));
    char *tmp = fgets(c, sizeof(c), pfile);
    while (tmp != NULL)
    {
      printf("DEBUG A: RECEIVED c: %s", c);
      // printf("DEBUG A: LEN c: %ld\n", sizeof(c));
      result=write(fd[1], &c, 255);
      if (result!=255)
      { 
        printf("DEBUG A: result error Thread A!\n");
        perror ("write"); 
        exit (2);
      }
      else
      {
        sem_post(&(p->sem_B));
        break;
      }
    }
    if (tmp == NULL)
    {
      printf("DEBUG A: Closing data.txt!\n");
      fclose(pfile);
      result=write(fd[1], &eof, 255);
      sem_post(&(p->sem_B));
      printf("/****** END OF THREAD A ******/\n");
      return 0;
    }
  }
}


void *ThreadB(void *params)
{
  ThreadParams *p = params;
  printf("/****** RUNNING THREAD B ******/\n");
  //TODO: add your code
  printf("\nthread B reading from thread A\n");
  
  while(1)
  {
    sem_wait(&(p->sem_B));
    char ch;
    int result;
    int index=0;
    char buff[255];
    result = read(fd[0], buff, 255);
    // printf("DEBUG B: result: %d\n", result);
    printf("DEBUG B: buff: %s\n", buff);
    if (result != 255)
    {
      printf("DEBUG B: result error Thread B!\n");
      perror("read");
      exit(4);
    }

    if (!strcmp(buff, "EOF"))
    {
      printf("DEBUG B: reading pipe has completed\n");
      // pthread_mutex_lock(&p->lock);
      // insert_item(params, '\0');
      insert_item(params, 2);
      // pthread_mutex_unlock(&p->lock);
      sem_post(&(p->sem_C));
      break;
    }

    while(buff[index] != '\0')
    {
      // pthread_mutex_lock(&p->lock);
      if (counter >= MAX_BUFFER-10)
      {
        printf("DEBUG B: Buffer is almost full!\n");
        // pthread_mutex_unlock(&p->lock);
        sem_post(&(p->sem_C));
        break;
      }
      
      insert_item(params, buff[index]);
      // pthread_mutex_unlock(&p->lock);
      index++;
    }
    // insert_item(params, buff[index]);
    sem_post(&(p->sem_C));
  }
  printf("/****** END OF THREAD B ******/\n");
}


void *ThreadC(void *params)
{
  ThreadParams *p = params;
  char item='0';
  static char tmp_buf[400];

  printf("/****** RUNNING THREAD C ******/\n");
  //TODO: add your code
  printf("\nThread C read from Thread B\n");

  FILE * pfile_out;
  pfile_out = fopen("output.txt", "w");
  if (pfile_out == NULL)
  {
    printf("ERROR A: FAILED TO OPEN FILE!\n");
    exit(1);
  }

  int write_to_file = 0;
  char tmp;
  while(1)
  {
    sem_wait(&(p->sem_C));
    int ii=0;
    Max_counter = counter;
    // pthread_mutex_lock(&p->lock); // Lock Mutex
    while(counter)
    {
      if (remove_item(params, &tmp) == 0)
      {
        tmp_buf[ii] = tmp;

        if (write_to_file == 1 && tmp_buf[ii] != 2)
          fwrite(&tmp_buf[ii], sizeof(tmp_buf[ii]), 1, pfile_out);
        if (strstr(tmp_buf, "end_header") != NULL && write_to_file == 0)
        {
          printf("end_header detected!!!\n");
          printf("tmp_buf: %s\n", tmp_buf);
          write_to_file = 1;
          ii=0;
        }
      }
      ii++;
    }

    // if (counter == 0 && tmp_buf[0] == 'E' && tmp_buf[1] == 'O' && tmp_buf[2] == 'F')
    printf("tmp_buf[0]: %d   ", tmp_buf[0]);
    printf("tmp_buf[0]: %c\n", tmp_buf[0]);
    if (counter == 0 && tmp_buf[0] == 2)
    {
      printf("/****** END OF THREAD C ******/\n");
      printf("DEBUG C: Final counter: \n%d\n", ii);
      printf("DEBUG C: Final buffer: \n%s\n", tmp_buf);
      sem_close(&(p->sem_A));
      sem_close(&(p->sem_B));
      sem_close(&(p->sem_C));
      exit(0);
    }
    else
    {
      printf("DEBUG C: counter: %d\n", counter);
      printf("DEBUG C: tmp_buf[ii]: %d\n", tmp_buf[ii]);
      sem_post(&p->sem_A);
    }
  }
}

/* Add an item to the buffer */
int insert_item(ThreadParams *p, u_int8_t item) {
   /* When the buffer is not full add the item
      and increment the counter*/
  // sleep(0.1);
  if(counter < MAX_BUFFER) 
  {
    p->message[counter] = (int)item;
    counter++;
    return 0;
  }
  else 
  { /* Error the buffer is full */
    return -1;
  }
}

/* Remove an item from the buffer */
int remove_item(ThreadParams *p, u_int8_t *item)
{
  /* When the buffer is not empty remove the item
     and decrement the counter */
  static int tracker=0;
  if(counter > 0)
  {
    // *item = buffer[(Max_counter-counter-1)];
    *item = p->message[(Max_counter-counter)];
    counter--;
    return 0;
  }
  else 
  { /* Error buffer empty */
    return -1;
  }
}



