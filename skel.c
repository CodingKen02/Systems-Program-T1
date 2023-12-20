/*
----------------------------------------------------------
Program: skel.c
Date: September 19, 2023
Student Name & NetID: Kennedy Keyes kfk38
Description: This program produces arithmetic flash cards and stores 
performance data. In broad strokes, the program should do the following: 
1. Load the current performance data from file 
a. If no data file exists, it should set performance data to 0 questions answered of 0 
questions attempted. 
2. The program should prompt the user for the range of numbers to include in the flash cards. This 
can be as easy as asking how many digits you want each number to be. 
3. The program should prompt the user for the total number of flash cards to generate. 
4. For each flash card, 
a. Generate two numbers in the proper range 
b. Generate the operation to be used (+, -, *) 
c. Print the problem to the screen 
d. Give the user 10 seconds to answer the question 
e. Update the performance data 
5. After all the flash cards have been presented, save the updated performance data to a file
----------------------------------------------------------
*/

/* Problem 1 – complete the list of include files*/
/* All of the includes for the project go here 
     those includes must support signals, interval 
     timers, read, open, and close. 
     You will also need support for error numbers */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/time.h>
#include <string.h>
#include <errno.h>
#include <math.h>
#include <fcntl.h>

#define BUF_SIZE 1024
int timedOut; // this integer represents a C Boolean value that is set to “true” if the SIGALRM is received 

/* ^ Problem 2 */ 
/* The PrintS function takes a constant c-string 
     and outputs it to STDOUT using write feel free 
     to use string functions like strcmp, strcat, etc 
     to make this one work. PrintS returns 0 on failure 
     and 1 on success */
int PrintS(const char *str)
{
    ssize_t len = strlen(str);
    if (write(STDOUT_FILENO, str, len) != len)
    {
        return 0; // return 0 on failure
    }
    return 1; // return 1 on success
}

/* ^ Problem 3 – implement the signalHandler */
/* implement the signal handler below ... 
     use the provided prototype It must handle 
     two signals, SIGINT and SIGALRM if the 
     signal handler receives SIGINT prompt the 
     user with “Exit (Y/n)?”  on yes, exit successfully, 
     on no, just return from the handler 

     on SIGALRM, set the timedOut flag to 1 */

void signalHandler(int sig)
{
    if (sig == SIGINT)
    {
        char response;
        PrintS("Exit (Y/n)? ");
        ssize_t bytesRead = read(STDIN_FILENO, &response, 1);
        if (bytesRead > 0 && (response == 'Y' || response == 'y'))
        {
            exit(EXIT_SUCCESS);
        }
    }
    else if (sig == SIGALRM)
    {
        timedOut = 1;
    }
}

int main(int argc, char *argv[])
{
     /* Problem 4a - Declare the necessary variables, question count, performance data, digits in each number, the operation, answers, file descriptors, etc. */
    int total;
    int digits;
    int answer;
    int response;
    int correct = 0;
    int attempted = 0;
    int operation;
    int fd;
    FILE *performanceData;

     /* Problem 4b - Declare the structures for signals and timers */
    struct sigaction sigAction;
    struct itimerval timerValue;

     /* Problem 5a - Initialize structures for signals and timers make certain to actually set a mask this time 
     add both SIGINT and SIGALRM to the mask - Initialize the structure for the signal handlers */
    sigemptyset(&sigAction.sa_mask);
    sigAction.sa_flags = 0;
    sigAction.sa_handler = signalHandler;

     /* Problem 5b - Initialize the structure for the 10 sec timer */
    timerValue.it_value.tv_sec = 10;
    timerValue.it_value.tv_usec = 0;
    timerValue.it_interval.tv_sec = 0;
    timerValue.it_interval.tv_usec = 0;

     /* Problem 5c OPTED-OUT*/

     /* Problem 6 - Associate the SIGINT and SIGALRM with their signal handler */
    sigaction(SIGINT, &sigAction, NULL);
    sigaction(SIGALRM, &sigAction, NULL);

     /* Problem 7a - Open the performance data file */
     performanceData = fopen("perf.dat", "r");
     if (performanceData == NULL)
     {
          correct = 0;
          attempted = 0;
     }
     else
     {

     /* Problem 7b - if the file opens, read the performance data */
     fd = open("perf.dat", O_RDONLY);
     if (fd != -1)
     {
          char buffer[BUF_SIZE];
          ssize_t bytesRead = read(fd, buffer, BUF_SIZE - 1);
     if (bytesRead > 0)
     {
          buffer[bytesRead] = '\0'; // null-terminate the string
          sscanf(buffer, "%d %d", &correct, &attempted);
     }
     /* Problem 7c - if the file opens, close the file */
     close(fd);
     }
     }

     /* Problem 7d - Display the current performance data */
     char performanceMessage[BUF_SIZE];
     snprintf(performanceMessage, BUF_SIZE, "Performance Data: %d questions answered of %d questions attempted.\n", correct, attempted);
     write(STDOUT_FILENO, performanceMessage, strlen(performanceMessage));
     /* Note: For this section of code, you are free to use printf and scanf.*/

     /* Problem 8 - Prompt the user for the total number of flashcards and digits */
     printf("Enter the total number of flashcards to generate: ");
     scanf("%d", &total);
     printf("Enter the number of digits for each number: ");
     scanf("%d", &digits);

     /* This is the main program loop, in this loop we will randomly generate the questions, 
     prompt the user for answers, verify the answers, and manage the question timer. */

     /* Problem 9 – A loop goes here … either for or while
     for question := 0 to total do */
     for (int question = 0; question < total; question++)
    {

     /* Problem 10 – output the current question.
     You should format the question similar to
     #question:
     ABC
     oDEF
     -------

     where o is +, - , or *. */
          printf("\n#question %d:\n", question + 1);

          /* Generate two random numbers */
          int num1 = rand() % (int)(pow(10, digits));
          int num2 = rand() % (int)(pow(10, digits));

          /* Generate a random operation (+, -, *) */
          operation = rand() % 3;
          char opChar;

          if (operation == 0)
          {
               answer = num1 + num2;
               opChar = '+';
          }
          else if (operation == 1)
          {
               answer = num1 - num2;
               opChar = '-';
          }
          else
          {
               answer = num1 * num2;
               opChar = '*';
          }

          printf("%*d\n%c%*d\n-------\n", digits, num1, opChar, digits, num2);

          /* we will set the timedOut flag to 0, since it hasn't yet */
          timedOut = 0;

          /* Problem 11 – set the interval timer to go off in 10 seconds. */
          setitimer(ITIMER_REAL, &timerValue, NULL);

          /* Problem 12 - Read the user's response */
          if (scanf("%d", &response) != 1)
          {
               /* Problem 13 - Handle interrupted read/scanf */
               if (errno == EINTR)
               {
                    if (timedOut)
                    {
                         printf("Time's up. Moving to the next question.\n");
                         continue;
                    }
                    else
                    {
                         printf("Restarting the timer...\n");
                         continue;
                    }
               }
               perror("scanf");
               exit(EXIT_FAILURE);
          }

          /* Disable the timer */
          timerValue.it_value.tv_sec = 0;
          setitimer(ITIMER_REAL, &timerValue, NULL);

          /* Recall, you should test errno against EINTR to determine if the scanf/read failed due to being interrupted by a signal. This block of code looks similar to 

          if scanf/read fails
          {
          if (errno == EINTR)
          {
               if (timedOut)
               {
                    Inform the user they have run out of time for the question 
                    jump back to the top of the for loop and create a new question
               }  
          Otherwise restart the read/scanf
          }
          perror(“read/scanf”);
          exit(EXIT_FAILURE);
          } 
          */

          /* Problem 14  */
          /* check the answer  ... this is just one way of doing it */
          attempted++;

          if (response == answer)
          {
               printf("Correct!\n");
               correct++;
          }
          else
          {
               printf("Attempted. The answer was %d.\n", answer);
          }
     }

          /* Problem 15 - Save the updated performance data to a file */
          performanceData = fopen("perf.dat", "w");
          if (performanceData != NULL)
          {
               fprintf(performanceData, "%d %d", correct, attempted);
               fclose(performanceData);
          }
          else
          {
               perror("Error writing performance data");
          }

     printf("Final score: %d out of %d\n", correct, total);

     exit(EXIT_SUCCESS);
}
