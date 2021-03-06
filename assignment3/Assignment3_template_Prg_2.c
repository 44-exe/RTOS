/**********************************************************************************

       *************NOTE**************
This is a template for the subject of RTOS in University of Technology Sydney(UTS)
Please complete the code based on the assignment requirement.

Assignment 3 Program_2 template
Author: Jeong Bin Lee
Date: 2022.05.05

**********************************************************************************/
/*
  To compile prog_2 ensure that gcc is installed and run the following command:
  gcc -Wall -O2 program_2.c -o prog_2 -lpthread -lrt
*/

#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h>
#include <semaphore.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <signal.h>

//Number of pagefaults in the program
int pageFaults = 0;

//Function declaration
void SignalHandler(int signal);

/**
 Main routine for the program. In charge of setting up threads and the FIFO.

 @param argc Number of arguments passed to the program.
 @param argv array of values passed to the program.
 @return returns 0 upon completion.
 */
int main(int argc, char* argv[])
{
  //Register Ctrl+c(SIGINT) signal and call the signal handler for the function.
  //add your code here
  signal(SIGINT, SignalHandler);
  
  if (argc < 2)
  {
    fprintf(stderr,"usage: requires <int> (for example: ./Assignment3_Prg2 4)\n");
    fprintf(stderr,"%s \n", *argv);
    return -11;
  }

  int i;
  // reference number
  int REFERENCESTRINGLENGTH=24;
  //Argument from the user on the frame size, such as 4 frames in the document
  int frameSize = atoi(argv[1]);
  //Frame where we will be storing the references. -1 is equivalent to an empty value
  uint frame[REFERENCESTRINGLENGTH];
  //Reference string from the assignment outline
  int referenceString[24] = {7,0,1,2,0,3,0,4,2,3,0,3,0,3,2,1,2,0,1,7,0,1,7,5};
  //Next position to write a new value to.
  int nextWritePosition = 0;
  //Boolean value for whether there is a match or not.
  bool match = false;
  //Current value of the reference string.
  int currentValue;

  //Initialise the empty frame with -1 to simulate empty values.
  for (i = 0; i < frameSize; i++)
  {
    frame[i] = -1;
  }

  printf("\tFault Status\tFault Number\tFrame State\n");
  
  //Loop through the reference string values.
  for (i = 0; i < REFERENCESTRINGLENGTH; i++)
  {
	  //add your code here
	  currentValue = referenceString[i];
    for (int j = 0; j < frameSize; j++)
	  {
	    if (frame[j] == referenceString[i])
	    {
	      //hit
		    match = true;
		    printf("\thit \t\tframe[%d]\t%d\n", j, frame[j]);
	    }
	  }

	  if (match == true)
	  {
	    match = false;
	  }
	  else
	  {
	    frame[nextWritePosition] = currentValue;
	    printf("\tfault \t\tframe[%d]\t%d\n", nextWritePosition, frame[nextWritePosition]);
	    pageFaults++;
	    nextWritePosition++;
	    if (nextWritePosition >= frameSize) //loop around the FIFO
	    {
        nextWritePosition = 0;
	    }
	  }
  }

  //Sit here until the ctrl+c signal is given by the user.
  while(1)
  {
    sleep(1);
  }

  return 0;
}

/**
 Performs the final print when the signal is received by the program.

 @param signal An integer values for the signal passed to the function.
 */
void SignalHandler(int signal)
{
  printf("\nTotal page faults: %d\n", pageFaults);
  exit(0);
}


// End of File
