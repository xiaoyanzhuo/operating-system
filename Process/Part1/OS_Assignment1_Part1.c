#include <stdio.h>
#include <ctype.h>
#include <limits.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>


#define   TotalTrial  1   ////numbers of sub-process want to be created

void prog();
void b();

int main()
{
	prog();
    return 0;
}

void prog()
{
	int trial = 0;
    pid_t  pid;

    while(trial < TotalTrial) {
    	trial ++;
    	pid = fork();         //create sub-process 
        if (pid < 0) {
            perror("fork");
            exit(EXIT_FAILURE);
        }                          
    	if (pid == 0){
    		printf("  **  Child Running **\n");  //display "Child Running"
    		b();                                 //Part1(b), another program b
    		exit(EXIT_SUCCESS);                  //exit 
    	}
    	else{
    		wait(NULL);                          //parents process wait until sub-process terminates
    		printf("   ** Parent Running @ trial # %d **\n", trial);
    	}
    }
}

void b()   //excute cd, pwd, ls
{
    chdir("..");  // execute "cd ..", return to the previous directory
    printf("cd executed, change directory \n");

    int idx;
    char* args[][2] = {
        { "/bin/pwd", NULL},    // execute "pwd"
        { "/bin/ls", NULL},     // execute "ls"
    };
    for (idx = 0; idx < 2; idx ++){
        if (0 == fork()) continue;
        execve(args[idx][0], &args[idx][0], NULL);
        if (-1 == (execve(args[idx][0], &args[idx][0], NULL)) )
        {
        perror( "execve" );
        exit( EXIT_FAILURE);
        } 
    }

}
//ref： http://c.biancheng.net/cpp/html/305.html
//https://stackoverflow.com/questions/9859903/using-the-exec-family-to-run-the-cd-command
//https://stackoverflow.com/questions/36908660/exec-cd-and-ls-and-fork-call-c
//https://support.sas.com/documentation/onlinedoc/sasc/doc/lr2/execv.htm (execv)
//http://pages.cs.wisc.edu/~remzi/OSTEP//cpu-api.pdf
//http://www.cs.ecu.edu/karl/4630/spr01/example1.html (An example using fork, execv and wait)
