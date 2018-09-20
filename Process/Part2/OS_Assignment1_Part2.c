#include <stdio.h>
#include <ctype.h>
#include <limits.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

void ProcessDouble(int n);

int main(int argc, char *argv[])
{
	if( argc != 2 ) {
		printf("Please enter the correct input(a number):\n"); //display if incorrect input
		exit(0);
	}

	int i;
	int proc_num = 2 * atoi(argv[1]);      // to create 2*n process
    for(i = 0; i < proc_num; i++) {
    	pid_t res = fork();
        if (res < 0) {
            perror("fork");
            exit(EXIT_FAILURE);
        }
		if(res == 0) {
			ProcessDouble(i);
			exit(0);
		}
		else{
			wait(NULL);    //wait until sub-process done
		}
    }
    return 0;
}

void ProcessDouble(int n) {
	int pid = getpid();
    printf("I am process %d and my process ID is %d\n", n + 1, pid);  //display process ID
}
