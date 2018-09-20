/*
 *  Created on: Dec 6, 2017
 *  Author: Xiaoyan Zhuo
 *  Client.c 
 *  For OS Assignment4. Client sends commands of 'list', 'cd', 'do_nothing' and 'quit'.
                        The excution results at the server will display in the client. -RPC.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/resource.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <getopt.h>
#include <fcntl.h>
#include <stdbool.h>

#define OK 	1
#define NOT_OK	-1

#define TRUE 	1
#define FALSE	0

#define TCP	2
#define UDP	1

#define BUFSIZE4	4
#define BUFSIZE512	512
#define BUFSIZE1024	1024
#define BUFSIZE4096	4096
#define COMMANDSIZE	10
#define FILENAMESIZE	25
#define FQDNBUFSIZE	100

#define TIMEOUT_SECONDS	3 //for adding timeout to client
#define SOMETHING_THERE	1
#define NOTHING_THERE	0

#define CMD_LIST	"list"
#define CMD_CD		"cd"
#define CMD_QUIT	"quit"
#define CMD_NOTHING	 "do_nothing"

#define SHOWPROMPT	write (1, "\nEnter Command (list,cd,quit)-> ", 40);

#define E_COMMAND_NOTFOUND	-1
#define E_COMMAND_INCOMPLETE	-2
#define RECEIVER_HOST "anaconda2.uml.edu"

int sizeOfTheMessage = BUFSIZE1024;
int typeOfSocketToCreate = UDP;
unsigned short controlPort = 24613; 
unsigned short datPrt;
int AnyThere(int);

void error(const char *msg)
{
    perror(msg);
    exit(0);
}

typedef struct Command 
{
        const char * cmdstr;
        int (*cmdfunc) (char *, const int *);
}command;

// The command list function
int ProcessCommandList (char *ignore, const int *cntrlscktFD){
	int numBytRd;
	int numBytWrtn;
	char tmpBuf [BUFSIZE1024];
	unsigned short datPrt;
	struct sockaddr_in srvrDatAdd;
	int datScktFD;
	struct hostent *serverDat;//resolve hostname
	if ((numBytWrtn = write (*cntrlscktFD, CMD_LIST, strlen (CMD_LIST))) < 0) {
		perror ("write");
		close (*cntrlscktFD);
		exit (EXIT_FAILURE);
	}			
	memset ((void *) tmpBuf, 0x00, BUFSIZE1024);
	if ((numBytRd = read (*cntrlscktFD, tmpBuf, BUFSIZE1024)) < 0) {
		perror ("read");
		close (*cntrlscktFD);
		exit (EXIT_FAILURE);
	}
	if (!strncasecmp (tmpBuf, "OK", 2)) {
		datPrt = atoi (& tmpBuf [3]);
	}

	printf ("Data Port returned by Server: %d\n", datPrt);
	if ((datScktFD = socket (AF_INET, SOCK_STREAM, 0)) < 0) {
		perror ("socket");	
		return NOT_OK;
	}

	serverDat = gethostbyname(RECEIVER_HOST);
	bzero((char *) &srvrDatAdd, sizeof(srvrDatAdd));
	srvrDatAdd.sin_family = AF_INET;
	bcopy((char *)serverDat->h_addr, (char *)&srvrDatAdd.sin_addr.s_addr,serverDat->h_length);
	srvrDatAdd.sin_port = htons (datPrt);		
	sleep (1);
	if (connect (datScktFD, (struct sockaddr *)&srvrDatAdd, sizeof (srvrDatAdd)) < 0) {
		perror ("connect");
		close (datScktFD);
		return NOT_OK;
	}
	
	memset ((void *)tmpBuf, 0x00, BUFSIZE1024);
	while (read(datScktFD, tmpBuf, BUFSIZE1024)) {
		write (1, tmpBuf, BUFSIZE1024);
		memset ((void *)tmpBuf, 0x00, BUFSIZE1024);
	}	
	close (datScktFD);
return OK;
}

//Function to change the directory
int ProcessCommandCD (char *path, const int *cntrlscktFD) {
	int numBytRd;
	int numBytWrtn;
	char tmpBuf [BUFSIZE1024];
	
	memset ((void *) tmpBuf, 0x00, BUFSIZE1024);
	sprintf (tmpBuf, "%s %s", CMD_CD, path);
	if ((path) && (path [0] != '\0')) {
		if ((numBytWrtn = write (*cntrlscktFD, tmpBuf, strlen (tmpBuf))) < 0) {
			perror ("write");
			close (*cntrlscktFD);
			exit (EXIT_FAILURE);
		}			
		memset ((void *) tmpBuf, 0x00, BUFSIZE1024);
		if ((numBytRd = read (*cntrlscktFD, tmpBuf, BUFSIZE1024)) < 0) {
			perror ("read");
			close (*cntrlscktFD);
			exit (EXIT_FAILURE);
		}						
		printf ("\n%s\n", tmpBuf);
		if (!strncasecmp (tmpBuf, "OK", 2)) {
			return OK;
		}
	} 
	else {
		return E_COMMAND_INCOMPLETE;
	}	
}

// Function for 'do nothing'
int ProcessCommandNothing (char *ignore, const int *cntrlscktFD){
	int numBytRd;
	int numBytWrtn;
	char tmpBuf [BUFSIZE1024];
	
	if ((numBytWrtn = write (*cntrlscktFD, CMD_NOTHING, strlen (CMD_NOTHING))) < 0) {
		perror ("write");
		close (*cntrlscktFD);
		exit (EXIT_FAILURE);
	}
			
	memset ((void *) tmpBuf, 0x00, BUFSIZE1024);
	if ((numBytRd = read (*cntrlscktFD, tmpBuf, BUFSIZE1024)) < 0) {
		perror ("read");
		close (*cntrlscktFD);
		exit (EXIT_FAILURE);
	}						
	printf ("\n%s\n", tmpBuf);
return OK;
}



// Function to close the client connection
int ProcessCommandQuit (char *ignore, const int *cntrlscktFD){
	int numBytRd;
	int numBytWrtn;
	char tmpBuf [BUFSIZE1024];

	while (1) {
		if ((numBytWrtn = write (*cntrlscktFD, CMD_QUIT, strlen (CMD_QUIT))) < 0) {
			perror ("write");
			close (*cntrlscktFD);
			exit (EXIT_FAILURE);
		}			
		memset ((void *) tmpBuf, 0x00, BUFSIZE1024);
		if ((numBytRd = read (*cntrlscktFD, tmpBuf, BUFSIZE1024)) < 0) {
			perror ("read");
			close (*cntrlscktFD);
			exit (EXIT_FAILURE);
		}					
		printf ("\n%s\n", tmpBuf);
		if (!strncasecmp (tmpBuf, "OK", 2)) {
			close (*cntrlscktFD);
			exit (EXIT_SUCCESS);
		}
	}
return OK;
}

command commands [] = {
        {CMD_LIST, ProcessCommandList},
        {CMD_NOTHING, ProcessCommandNothing},
        {CMD_CD, ProcessCommandCD},
        {CMD_QUIT, ProcessCommandQuit},
        {0, 0},
};

// Function to process the commands
int ProcessTheCommand (const char *command, const int *cntrlscktFD)
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

char * ReadTheCommand (char *tmpBuf) {
        fgets (tmpBuf, 50, stdin);
		// gets (tmpBuf);
        int len = strlen (tmpBuf);
        tmpBuf [len -1] = '\0';
	return tmpBuf;
}

// Main Function
int main (int argc, char *argv []) {
	int cntrlscktFD;
	int datScktFD;
	struct sockaddr_in srvrCntrlAdd;
	struct sockaddr_in srvrDatAdd;
	socklen_t serverLen;
	struct hostent *server;

	printf ("\n Server's Address: %s\n", RECEIVER_HOST);	
	printf ("\n Server's Port Number: %d\n", htons (controlPort));

	if ((cntrlscktFD = socket (AF_INET, SOCK_STREAM, 0)) < 0) {
		perror ("socket");
		exit (EXIT_FAILURE);
	}

	//fill the socket
	server = gethostbyname(RECEIVER_HOST);
	if (server == NULL) {
		fprintf(stderr,"ERROR, no such host\n");
		exit(0);
	}

	bzero((char *) &srvrCntrlAdd, sizeof(srvrCntrlAdd));
	srvrCntrlAdd.sin_family = AF_INET;
	bcopy((char *)server->h_addr, (char *)&srvrCntrlAdd.sin_addr.s_addr,server->h_length);
    srvrCntrlAdd.sin_port = htons(controlPort);

    if (connect(cntrlscktFD,(struct sockaddr *)&srvrCntrlAdd,sizeof(srvrCntrlAdd)) < 0)
    	error("ERROR connecting");

	char tmpBuf [BUFSIZE1024];
	while (1) {
		int retValue;
		memset ((void *) tmpBuf, 0x00, BUFSIZE1024);
		SHOWPROMPT;
		ReadTheCommand (tmpBuf);
		retValue = ProcessTheCommand (tmpBuf, &cntrlscktFD);
		if (retValue == E_COMMAND_INCOMPLETE) {
			printf ("What? - Command is Incomplete...\n");
		}
		else if (retValue == 0) {
			printf ("What? - Command Not Found...\n");
		}
	} 
	close (cntrlscktFD);
	return EXIT_SUCCESS;
}
// Add a timeout in client, after 3 seconds no response from server, clients reports.
int AnyThere(int s)
{
fd_set read_mask;
struct timeval timeout;
int ret;
timeout.tv_sec=TIMEOUT_SECONDS;
timeout.tv_usec=0;
FD_ZERO(&read_mask);
FD_SET(s,&read_mask);
if((ret=select(32,&read_mask,0,0,&timeout))<0)
	perror ("select");
return((ret=0)? NOTHING_THERE : SOMETHING_THERE);
}
