/*
 * Shell.c
 *
 *  Created on: Feb 19, 2019
 *      Author: harshit
 */
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#define MAXLINE 80 /* The maximum length command */

//A utility function to print the errors.
void printError(char * ERROR_MESSAGE){
	write(STDERR_FILENO, ERROR_MESSAGE, strlen(ERROR_MESSAGE));
}

// A utility function to tokenize the  string in spaces.
int tokenizeString(char *buffer, char * args[]){
	int position=0;
	char* token = strtok(buffer, " \t\n");          //  divides the string in every space or tab or newline.

	while (token != NULL) {
		args[position] = token;
		++position;
		token = strtok(NULL, " \t\n");
	}
	args[position] = NULL;       // Adds the NULL character at the  last.
	return position;             // returns the position of last character.
}

int main(void)
{
	char buffer[MAXLINE];
	char *args[MAXLINE/2 + 1]; /* command line with max 40 arguments */
	int should_run = 1; /* flag to determine when to exit program */
	int lastElementPosition = 0;    // Determining the position of the last element for concurrency
	int concurrent = 0;         // Flag to determine the concurrent behavior of the parent-child process.
	pid_t pid;                  // variable holding the Process id of the newly created child process.
	int status;                 // Variable holding the exit status of the child process.

	printf("CS149 Shell from Harshit Agarwal\n");
	// The following loop creates a REPL for the user to execute in CML.
	while (should_run) {
		printf("Harshit-866>");
		fflush(stdout);

		// Reads the input from the user.
		if(fgets(buffer,MAXLINE, stdin) == NULL) {
			printError("The shell needs an input to execute.\n");
			_exit(1);
		}
		// Gets the last element and initializes all the  command line arguments to  the args variables.
		lastElementPosition = tokenizeString(buffer,args);

		// exits the program.
		if(strcmp(args[0], "exit") == 0){
			should_run = 0;
			_exit(1);
		}

		//  A check for the concurrent behavior
		if(strcmp(args[lastElementPosition-1],"&") == 0){
			concurrent =1;
			args[lastElementPosition-1] = NULL;
		}

		// Creates  a new child Process
		pid = fork();

		// prints  an error if the child process is not   created.
		if(pid<0){
			printError("The process could not be completed try again.\n");
			_exit(-1);
		}
		// if it is a child  process then execute and replace the child process.
		else if (pid ==0 ){
			execvp(args[0],args);          // the child process is replaced with the new process.
			printError("The input command is invalid try again with a new command\n");     // prints an error if the execv fails.
			_exit(1);

		}else{
			if(concurrent == 0){
				waitpid(pid, &status,0);        // the parent process waits for the child  process to end. If & then the parent doesnot wait for child making the child a orphan process.
			}
		}
	}
	return 0;
}
