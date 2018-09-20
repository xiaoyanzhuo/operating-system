/*
 *  Created on: Dec 6, 2017
 *  Author: Xiaoyan Zhuo
 *  Serve.c 
 *  For OS Assignment4. Server can serve multiple clients simultaneously.
                        Execute commands of 'list', 'cd', 'do_nothing' and 'quit'.
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <dirent.h>
#include <errno.h>
#include <stdbool.h>
#include <signal.h>

#define OK 	1
#define NOT_OK	-1
#define ERROR_SYS -2

#define TRUE 	1
#define FALSE	0

#define TCP	1
#define UDP	2

#define MAX(a,b) ((a) > (b) ? a : b)
#define LOCALPATH       "/tmp/Assignment"

#define BUFFSIZE1024	1024
#define FQDNBUFSIZE	100
#define COMMANDSIZE	10
#define FILENAMESIZE	25

#define CMD_LIST	"list"
#define CMD_CD		"cd"
#define CMD_QUIT	"quit"
#define CMD_NOTHING		"do_nothing"

// signal handler: terminate server process(ctr+c)
void sigchld_handler(int s)
{
    while(wait(NULL) > 0);
}

/* Global Variables */
extern int errno;

typedef struct Command
{
	const char * cmdstr;
	int (*cmdfunc) (char *, const int *); 
}command;


unsigned short controlPort = 24613;
unsigned short datPrt;
static bool isDataScktCreated = false;

int PrcsNwCon (int clientFD, int clientNumber);
int ListeningSrvr (unsigned short);

// The command list function
int ProcessCommandList (char *igore, const int *cntrlscktFD) {
	char tmpBuf [BUFFSIZE1024];
	int numBytWrtn = 0;
	int returnValue = OK;
	DIR *dp = NULL;
        struct dirent *pDir = NULL;
	unsigned short datPrt = htons (IPPORT_USERRESERVED + getuid () + getpid ());

	printf("Receive command: list\n");

	memset ((void *) tmpBuf, 0x00, BUFFSIZE1024);
	sprintf (tmpBuf, "%s %d", "OK", datPrt);	

	if ((numBytWrtn = write (*cntrlscktFD, tmpBuf, BUFFSIZE1024)) < 0) {
		perror ("write");
		close (*cntrlscktFD);
		exit (EXIT_FAILURE);	
	}


	system ("ls -la > /var/tmp/temp");
	int fp;
	int datScktFD;
	int clientFD;
	struct sockaddr_in clntAddr;
	socklen_t clientLen;
	memset ((void *) &clntAddr, 0x00, sizeof (struct sockaddr_in));
	memset ((void *) tmpBuf, 0x00, BUFFSIZE1024);
	if ((fp = open ("/var/tmp/temp", 0)) >= 0) {
		datScktFD = ListeningSrvr (datPrt);
		if ((clientFD = accept (datScktFD, (struct sockaddr *)& clntAddr, &clientLen)) < 0) {
			perror ("accept");
			close (datScktFD);
			exit (EXIT_FAILURE);
		}
		while (read (fp, tmpBuf, 1024)) {
			write (clientFD, tmpBuf, 1024);	
			memset ((void *)tmpBuf, 0x00, 1024);
		}
		close (fp);
		system ("rm -f /var/temp");
		close (datScktFD);
		close (clientFD);
	} 
	else {
		sprintf (tmpBuf, "%s", strerror(errno));
		if ((numBytWrtn = write (*cntrlscktFD, tmpBuf, BUFFSIZE1024)) < 0) {
			perror ("write");
			close (*cntrlscktFD);
			exit (EXIT_FAILURE);	
		}
		returnValue = ERROR_SYS;
	}
return returnValue;
}

// Function for 'do nothing'
int ProcessCommandNothing (char *igore, const int *cntrlscktFD ) {
	char tmpBuf [BUFFSIZE1024];
	int numBytWrtn = 0;
	memset ((void *) tmpBuf, 0x00, BUFFSIZE1024);
	sprintf (tmpBuf, "%s", "Server is doing nothing\n");
	printf("I am doing nothing\n");
	if ((numBytWrtn = write (*cntrlscktFD, tmpBuf, BUFFSIZE1024)) < 0) {
		perror ("write");
		close (*cntrlscktFD);
		exit (EXIT_FAILURE);	
	}	
return OK;
}



//Function to change the directory
int ProcessCommandCD (char *path, const int *cntrlscktFD) {
	char tmpBuf [BUFFSIZE1024];
	int numBytWrtn = 0;
	int returnValue = OK;
	memset ((void *) tmpBuf, 0x00, BUFFSIZE1024);
	sprintf (tmpBuf, "%s", "OK");

	printf("Receive command: cd %s\n", path);

	if ((!strncmp (path, "..", 2)) || (!strncmp (path, "./", 2)) || (path [0] == '/')) {
                if (chdir (path) < 0) {
                	sprintf (tmpBuf, "%s", strerror (errno));
			returnValue = ERROR_SYS;
                }
	}       
        else {
                sprintf (tmpBuf, "%s%s", "./", path);
                if (chdir (tmpBuf) < 0) {
                	sprintf (tmpBuf, "%s", strerror (errno));
			returnValue = ERROR_SYS;
                }else {
			sprintf (tmpBuf, "%s", "OK");
		}
	}
	if ((numBytWrtn = write (*cntrlscktFD, tmpBuf, BUFFSIZE1024)) < 0) {
		perror ("write");
		close (*cntrlscktFD);
		exit (EXIT_FAILURE);	
	}
return returnValue;
}

// Function to close the client connection
int ProcessCommandQuit (char *igore, const int *cntrlscktFD) {
	char tmpBuf [BUFFSIZE1024];
	int numBytWrtn = 0;

	printf("Receive command: quit\n");

	memset ((void *) tmpBuf, 0x00, BUFFSIZE1024);
	sprintf (tmpBuf, "%s", "OK");
	if ((numBytWrtn = write (*cntrlscktFD, tmpBuf, BUFFSIZE1024)) < 0) {
		perror ("write");
		close (*cntrlscktFD);
		exit (EXIT_FAILURE);	
	}

	int lclSrvrFD;
        struct sockaddr_un lclSrvrAddr;
        if ((lclSrvrFD = socket (AF_LOCAL, SOCK_STREAM, 0)) < 0) {
        	perror ("local socket");
                exit (EXIT_FAILURE);
	}

        memset ((void *) &lclSrvrAddr, 0x00, sizeof (struct sockaddr_un));
        lclSrvrAddr.sun_family = AF_LOCAL;
        strcpy (lclSrvrAddr.sun_path, LOCALPATH);
        if (connect (lclSrvrFD, (struct sockaddr *) &lclSrvrAddr, sizeof (lclSrvrAddr)) < 0) {
        	perror ("local connect");
                exit (EXIT_FAILURE);
	}

	close (*cntrlscktFD);	
	close (lclSrvrFD);
	exit(EXIT_SUCCESS);
}

command commands [] = {
	{CMD_LIST, ProcessCommandList},
	{CMD_NOTHING, ProcessCommandNothing},
	{CMD_CD, ProcessCommandCD},
	{CMD_QUIT, ProcessCommandQuit},
	{0, 0},
};

// Function To parse the commands
int ParseTheCommand (const char *command, const int *cntrlscktFD)
{
	char commandString [COMMANDSIZE];
	char fileName [FILENAMESIZE];
	memset ((void *) commandString, 0x00, COMMANDSIZE); 	
	memset ((void *) fileName, 0x00, FILENAMESIZE); 		
	sscanf (command, "%s %s", commandString, fileName);
	int i;
	for (i = 0; commands[i].cmdstr; i++) {
		if (strcmp (commandString, commands[i].cmdstr) == 0)
			return commands[i].cmdfunc (fileName, cntrlscktFD);		
	}	

}

// Function to Create TCP Listening server
int ListeningSrvr (unsigned short port) {
	int srvrFD;
	struct sockaddr_in serverAddress;
	if ((srvrFD = socket (AF_INET, SOCK_STREAM, 0)) < 0) {
		perror ("socket");
		exit (EXIT_FAILURE);
	}
	int optval = 1;
	setsockopt(srvrFD, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof (optval));
	memset ((void *) &serverAddress, 0x00, sizeof (serverAddress));

	serverAddress.sin_family = AF_INET;
	// serverAddress.sin_addr.s_addr = htonl (INADDR_ANY);
	serverAddress.sin_addr.s_addr = INADDR_ANY;
	serverAddress.sin_port = htons (port);

	if (bind (srvrFD, (struct sockaddr *) &serverAddress, sizeof (serverAddress)) < 0) {
		perror ("bind");
		exit (EXIT_FAILURE);
	}	

	if (listen (srvrFD, 5) < 0) {
		perror ("listen");
		close (srvrFD);
		exit (EXIT_FAILURE);
	}
return srvrFD;
}

// Function to Create Local  TCP Listening server
int LocalListeningSrvr () {
        int lclSrvrFD;
        struct sockaddr_un lclSrvrAddr;

        if ((lclSrvrFD = socket (AF_LOCAL, SOCK_STREAM, 0)) < 0) {
                perror ("local socket");
                exit (EXIT_FAILURE);
        }
        memset ((void *) &lclSrvrAddr, 0x00, sizeof (lclSrvrAddr));
        unlink (LOCALPATH);
        lclSrvrAddr.sun_family = AF_LOCAL;
        strcpy (lclSrvrAddr.sun_path, LOCALPATH);	
	int i  =  strtol ("0777", 0, 8);
        chmod (LOCALPATH, i);     
        if (bind (lclSrvrFD, (struct sockaddr *) &lclSrvrAddr, sizeof (lclSrvrAddr)) < 0) {
                perror ("bind");
                exit (EXIT_FAILURE);
        }
        if (listen (lclSrvrFD, 5) < 0) {
                perror ("listen");
                close (lclSrvrFD);
                exit (EXIT_FAILURE);
        }
return lclSrvrFD;
}
// Main Function
int main (int argc, char *argv []) {
	int srvrFD, clientFD;
	struct sigaction sa;
	struct sockaddr_in clntAddr, serverAddress;     
	int lclSrvrFD, lclClntFD;
        struct sockaddr_un localclntAddr;
	pid_t childPID;
	int totalClient = 0;	
        int maxFD;
        fd_set rset;

	srvrFD = ListeningSrvr (controlPort); 
            lclSrvrFD = LocalListeningSrvr () ;
            FD_ZERO (&rset);

    sa.sa_handler = sigchld_handler; // reap all dead processes
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    if (sigaction(SIGCHLD, &sa, NULL) == -1) 
    {
    	perror("sigaction");
        exit(1);
    }

   	while (1) {    
             FD_SET (srvrFD, &rset);
             FD_SET (lclSrvrFD, &rset);
             maxFD = MAX (srvrFD, lclSrvrFD) + 1;
            if ((select (maxFD, &rset, NULL, NULL, NULL)) < 0) {
                     perror ("select");
                     exit (EXIT_FAILURE);
            }
   		if (FD_ISSET (srvrFD, &rset)) {
		char clntIPAdd [INET_ADDRSTRLEN];
	       	socklen_t clientLen;
      	if ((clientFD = accept (srvrFD, (struct sockaddr *)& clntAddr, &clientLen)) < 0) {
	      	perror ("accept");
	      	close (srvrFD);
       		exit (EXIT_FAILURE);
       	}

       	totalClient = totalClient + 1;

	if ((childPID = fork ()) == 0) { //child process
	  close (srvrFD); // close main server
	      	PrcsNwCon (clientFD, totalClient);
      		exit (EXIT_SUCCESS);
			}	
	close (clientFD); // Close new client socket
		}

                    if (FD_ISSET (lclSrvrFD, &rset)) {
                            socklen_t localClientLen;
                            if ((lclClntFD = accept (lclSrvrFD, (struct sockaddr *)& localclntAddr, &localClientLen)) < 0) {
                                    perror ("accept");
                                    close (lclSrvrFD);
                                    exit (EXIT_FAILURE);
                            }
                            totalClient = totalClient - 1;
                            printf ("Total Number of Active Clients: %d\n", totalClient);
                    }
	} 	
	// } 	
return EXIT_SUCCESS;
}

// Function to receive and reply the messages from client
int PrcsNwCon (int cntrlscktFD, int clientNumber) {	
	char tmpBuf [BUFFSIZE1024];
	int serverDataFD;
	static bool isDataScktCreated = false;	
	int numBytRd = 0;
	int numBytWrtn = 0;
	int retValue;
	printf ("Total Active Clients: %d\n", clientNumber);
	while (1) {
		memset ((void *)tmpBuf, 0x00, BUFFSIZE1024);
		numBytRd = read (cntrlscktFD, tmpBuf, BUFFSIZE1024);
		if (numBytRd == 0) {
			printf ("\nClient closed the connection\n");
			close (cntrlscktFD);
			return (EXIT_SUCCESS);
		}	
		else if (numBytRd < 0) {
			perror ("read");
			close (cntrlscktFD);
			exit (EXIT_FAILURE);	
		}
		retValue =  ParseTheCommand (tmpBuf, &cntrlscktFD);
	}
return (EXIT_SUCCESS);
}
