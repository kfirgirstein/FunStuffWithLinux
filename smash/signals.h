/*******************************************/
// signals.h
/*******************************************/

#ifndef _SIGS_H
#define _SIGS_H
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h> 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include "commands.h"

extern pid_t foreground_pid;

void SIGINT_handler(int signum);
void SIGTSTP_handler(int signum);
void SIGCHILD_handler(int signum);
#endif

