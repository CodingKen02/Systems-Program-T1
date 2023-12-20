#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/time.h>
#include <string.h>
#include <errno.h>

#define BUF_SIZE 1024

int timedOut;

/* Problem 2 - Implement PrintS using write */
int PrintS(const char *str)
{
    // Use the 'write' function to print the string 'str' to STDOUT
    // Return 1 on success and 0 on failure
    ssize_t len = strlen(str);
    if (write(STDOUT_FILENO, str, len) != len)
    {
        return 0; // Return 0 on failure
    }
    return 1; // Return 1 on success
}

/* Problem 3 - Implement the signal handler for SIGINT and SIGALRM */
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
    /* Problem 4 - Declare the necessary variables */
    int correct = 0;
    int total;
    int digits;
    int answer;
    int response;

    /* Problem 4a - Declare question count, performance data, digits in each number, the operation, answers, file descriptors, etc. */
    int questionsAnswered = 0;
    int questionsAttempted = 0;
    int operation;
    int fd;
    FILE *performanceDataFile;

    /* Problem 4b - Declare the structures for signals and timers */
    struct sigaction sigAction;
    struct itimerval timerValue;

    /* Problem 5 - Initialize structures for signals and timers */
    sigemptyset(&sigAction.sa_mask);
    sigAction.sa_flags = 0;

    /* Problem 5a - Initialize the structure for the signal handlers */
    sigAction.sa_handler = signalHandler;

    /* Problem 5b - Initialize the structure for the 10 sec timer */
    timerValue.it_value.tv_sec = 10;
    timerValue.it_value.tv_usec = 0;
    timerValue.it_interval.tv_sec = 0;
    timerValue.it_interval.tv_usec = 0;

    /* Problem 5c - Initialize the structure for the 0 sec timer (optional) */

    /* Problem 6 - Associate the SIGINT and SIGALRM with their signal handler */
    sigaction(SIGINT, &sigAction, NULL);
    sigaction(SIGALRM, &sigAction, NULL);

    /* Problem 7a - Open the performance data file */
    performanceDataFile = fopen("perf.dat", "r");
    if (performanceDataFile == NULL)
    {
        questionsAnswered = 0;
        questionsAttempted = 0;
    }
    else
    {
        /* Problem 7b - Read the performance data */
        fscanf(performanceDataFile, "%d %d", &questionsAnswered, &questionsAttempted);
        /* Problem 7c - Close the file */
        fclose(performanceDataFile);
    }

    /* Problem 7d - Display the current performance data */
    printf("Performance Data: %d questions answered of %d questions attempted.\n", questionsAnswered, questionsAttempted);

    /* Problem 8 - Prompt the user for the total number of flashcards and digits */
    printf("Enter the total number of flashcards to generate: ");
    scanf("%d", &total);
    printf("Enter the number of digits for each number: ");
    scanf("%d", &digits);

    /* Problem 9 - Main loop for generating flashcards and checking answers */
    for (int question = 0; question < total; question++)
    {
        /* Problem 10 - Output the current question */
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

        /* Reset the timedOut flag */
        timedOut = 0;

        /* Problem 11 - Set the timer for 10 seconds */
        setitimer(ITIMER_REAL, &timerValue, NULL);

        /* Problem 12 - Read the user's response */
        if (scanf("%d", &response) != 1)
        {
            /* Problem 13 - Handle interrupted read/scanf */
            if (errno == EINTR)
            {
                if (timedOut)
                {
                    printf("Time's up! Moving to the next question...\n");
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

        /* Problem 14 - Check the answer and update performance data */
        questionsAttempted++;

        if (response == answer)
        {
            printf("Correct!\n");
            questionsAnswered++;
        }
        else
        {
            printf("Wrong! The answer was %d.\n", answer);
        }
    }

        /* Problem 15 - Save the updated performance data to a file */
        performanceDataFile = fopen("perf.dat", "w");
        if (performanceDataFile != NULL)
        {
            fprintf(performanceDataFile, "%d %d", questionsAnswered, questionsAttempted);
            fclose(performanceDataFile);
        }
        else
        {
            perror("Error writing performance data");
        }

    printf("Final score: %d out of %d\n", questionsAnswered, total);

    exit(EXIT_SUCCESS);
}
