/*	smash.c
 main file. This file contains the main function of smash
 *******************************************************************/
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h> 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <exception>
#include "commands.h"
#include "signals.h"
#define MAX_LINE_SIZE 80
#define MAXARGS 20

char* L_Fg_Cmd;
char lineSize[MAX_LINE_SIZE];
//**************************************************************************************
// function name: main
// Description: main function of smash. get command from user and calls command functions
//**************************************************************************************
int main(int argc, char *argv[])
{
	char cmdString[MAX_LINE_SIZE];
	struct sigaction sa;
	// Restart the system call, if at all possible
	sa.sa_flags = SA_RESTART;
	// Block every signal during the handler
	sigfillset(&sa.sa_mask);
	// Intercept SIGINT
	sa.sa_handler = &SIGINT_handler;
	if (sigaction(SIGINT, &sa, NULL) == -1)
	{
		std::cerr << "Error: cannot handle SIGINT" << std::endl; // Should not happen
	}
	// Intercept SIGSTP
	sa.sa_handler = &SIGTSTP_handler;
	if (sigaction(SIGTSTP, &sa, NULL) == -1)
	{
		std::cerr << "Error: cannot handle SIGTSTP" << std::endl; // Should not happen
	}

	sa.sa_handler = &SIGCHILD_handler;
	if (sigaction(SIGCHLD, &sa, NULL) == -1)
	{
		std::cerr << "Error: cannot handle SIGTSTP" << std::endl; // Should not happen
	}

	// Init globals 
	L_Fg_Cmd = (char*) malloc(sizeof(char) * (MAX_LINE_SIZE + 1));
	if (L_Fg_Cmd == NULL)
		exit(-1);
	L_Fg_Cmd[0] = '\0';

	while (1)
	{
		printf("smash > ");
		if (NULL == fgets(lineSize, MAX_LINE_SIZE, stdin))
		{
			strcpy(lineSize, "quit");
		}
		strcpy(cmdString, lineSize);
		cmdString[strlen(lineSize) - 1] = '\0';
		// background command
		JobsList::GetInstance()->AddToHistory(cmdString);
		try
		{
			if (ReturnValue::Pass != BgCmd(lineSize))
				continue;
			// built in commands
			ExeCmd(lineSize, cmdString);
		} catch (std::exception &e)
		{
			perror(e.what());
			std::cout<<"HERE!!" <<std::endl;
		}

		/* initialize for next line read*/
		lineSize[0] = '\0';
		cmdString[0] = '\0';
		fflush(NULL);

	}
	return 0;
}

