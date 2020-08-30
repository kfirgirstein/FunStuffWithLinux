/*
 * JobsList.h
 *
 *  Created on: Apr 13, 2020
 *      Author: os
 */

#ifndef JOBSLIST_H_
#define JOBSLIST_H_
#include"jobprocess.h"
#include <string>
#include <sstream>
#include <vector>
#include <algorithm>
#include <fstream>
#include <sys/wait.h>
#include <unistd.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>

#define MAX_HISTORY 50
#define TIME_FOR_ENDING 5 //time to wait after SIGTERM, when "quit kill" is operated

extern pid_t foreground_pid;

enum ReturnValue{ Success=1,Pass=0,ERROR_VALUE=-1};


/*************************************************/
 //----- Class that defines The List of Jobs -----//
/*************************************************/

class JobsList
{
public:
	//----- Get Functions -----//
	std::vector<job_process>& GetJobs();
	std::vector<std::string>& GetHistory();
	static JobsList *GetInstance();

	//----- Delete Functions -----//
	static bool RemoveInstance();
	void KillAllJobs();
	void CleanFinished();

	//----- Jobs Functions -----//
	bool WaitJob(uint index);
	bool KillJob(uint index, int signum);
	bool AddJob(job_process&);
	bool WakeJob(uint index);
	static int WaitToProcess(pid_t pid);
	static bool SendSignalToProcess(pid_t pid,int signum);

	//----- Print Functions -----//
	std::string toString(void);

	//----- Other Functions -----//
	bool AddToHistory(char* newLine);
	int BiggestJobNumSuspended();

private:
	JobsList();
	static uint _instanceNumber;
	static JobsList *_instance;
	std::vector<job_process> _jobs;
	std::vector<std::string> _history_log;
	static std::string getSingalName(int sig_num);

};

#endif /* JOBSLIST_H_ */
