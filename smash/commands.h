//		commands.h

#ifndef _COMMANDS_H
#define _COMMANDS_H

#include "JobsList.h"
#include <unistd.h> 
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <ext/string_conversions.h>
#include <vector>
#include <string>
#include <fstream>
#include <iostream>


#define MAX_LINE_SIZE 80
#define MAX_ARG 20

static const std::vector<std::string> COMMANDS =
{ "cd", "pwd", "cp", "jobs", "history", "showpid", "fg", "bg", "quit", "diff" };

int ExeCmd(char* lineSize, char* cmdString);
bool CompareJobSuspend(job_process j1, job_process j2);
int BgCmd(char* lineSize);
void ExeExternal(char *args[MAX_ARG], char* cmdString,bool background=false);
int parseCommand(char* lineSize,char* args[]);
#endif

