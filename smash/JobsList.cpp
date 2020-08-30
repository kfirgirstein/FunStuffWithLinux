/*
 * JobList.h
 *
 *  Created on: Apr 13, 2020
 *      Author: os
 */
#include "JobsList.h"

/*************************************************/
 //----- Class that defines The List of Jobs -----//
/*************************************************/

JobsList* JobsList::_instance = 0;
uint JobsList::_instanseNumber = 0;


JobsList::JobsList(void)
{
}

/***************************/
//----- Get Functions -----//
/***************************/
std::vector<job_process>& JobsList::GetJobs()
{
	return this->_jobs;
}

std::vector<std::string>& JobsList::GetHistory()
{
	return this->_history_log;
}

JobsList *JobsList::GetInstance()
{
	if (!JobsList::_instance)
	{
		JobsList::_instance = new JobsList;
	}
	JobsList::_instanseNumber++;
	return _instance;
}


/******************************/
//----- Delete Functions -----//
/******************************/
bool JobsList::RemoveInstance()
{
	if ((--JobsList::_instanseNumber) <= 0)
	{
		delete JobsList::_instance;
	}
	return true;
}

void JobsList::KillAllJobs()
{
	int status;
	size_t listsize = this->_jobs.size();
	for (size_t i = 0; i < listsize; i++)
	{
		auto job = this->_jobs[i];
		std::cout << "[" << i + 1 << "] " << job.getCmd()
				<< " – Sending SIGTERM...";

		kill(job.getPid(), SIGTERM); //if not succeed then kill will happened after  5 seconds

		if (waitpid(job.getPid(), &status, WNOHANG) < 0)
		{
			std::cout << " Done." << std::endl;
			continue;
		}
		sleep(TIME_FOR_ENDING);
		if (waitpid(job.getPid(), &status, WNOHANG) < 0)
		{
			std::cout << " Done." << std::endl;
		}
		else
		{
			if(kill(job.getPid(), SIGKILL) == ReturnValue::ERROR_VALUE)
			{
				fprintf(stderr, "Error killing job %d - cannot send signal", int(i));
				continue;
			}
			std::cout << "(" << TIME_FOR_ENDING
					<< " sec passed) Sending SIGKILL… Done." << std::endl;

		}
	}

}

void JobsList::CleanFinished()
{
	int status;
	for (size_t index = 0; index < this->_jobs.size(); ++index)
	{
		if(ReturnValue::Pass == waitpid(this->_jobs[index].getPid(), &status, WNOHANG))
		{
			continue;
		}
		if (WIFEXITED(status) || WIFSIGNALED(status))
		{
			//std::cout << "["<<index+1<<"]"<< "Done \t" << this->_jobs[index].getCmd() <<std::endl;
			this->_jobs.erase(this->_jobs.begin() + index);
		}
	}
}


/****************************/
//----- Jobs Functions -----//
/****************************/
bool JobsList::WaitJob(uint index)
{
	//need to check if index is valid
	if (!this->WakeJob(index))
	{
		return false;
	}
	auto wait_stauts = JobsList::WaitToProcess(this->_jobs[index].getPid());
	if (WIFSTOPPED(wait_stauts))
	{
		this->_jobs[index].setStopped(true);
	}
	if (WIFEXITED(wait_stauts))
	{
		this->_jobs.erase(this->_jobs.begin() + index);
	}
	return true;
}

bool JobsList::KillJob(uint index, int signum)
{
	//need to check if index is valid
	if ((index < 0) || (this->_jobs.size() < index))
	{
		return false;
	}
	auto result = JobsList::SendSignalToProcess(this->_jobs[index].getPid(),
			signum);
	if (!result)
	{
		fprintf(stderr, "kill %d - cannot send signal", index);
		perror("");
		return false;
	}
	switch (signum)
	{
	case SIGSTOP:
	case SIGTSTP:
		this->_jobs[index].setStopped(true);
		break;
	case SIGCONT:
		this->_jobs[index].setStopped(false);
		break;
	case SIGTERM:
	case SIGKILL:
		this->_jobs.erase(this->_jobs.begin() + index);
		break;
	}
	return true;
}
bool JobsList::AddJob(job_process& j)
{
	this->_jobs.push_back(j);
	return true;
}

bool JobsList::WakeJob(uint index)
{
	if ((index < 0) || (this->_jobs.size() < index))
	{
		return false;
	}
	if (this->_jobs[index].isStopped())
	{
		if (false == this->KillJob(index, SIGCONT))
		{
			return false;
		}
		this->_jobs[index].setStopped(false);
	}
	return true;
}

int JobsList::WaitToProcess(pid_t pid)
{
	int wait_stauts;
	foreground_pid = pid;
	waitpid(pid, &wait_stauts, WUNTRACED);
	return wait_stauts;
}

bool JobsList::SendSignalToProcess(pid_t pid, int signum)
{
	if (kill(pid, signum))
	{
		return false;
	}
	std::cout << "smash > signal " << JobsList::getSingalName(signum)
			<< " was sent to pid " << pid << std::endl;
	return true;
}


/*****************************/
//----- Other Functions -----//
/*****************************/
bool JobsList::AddToHistory(char* cmdString)
{
	if (this->_history_log.size() >= MAX_HISTORY)
	{
		auto diff = this->_history_log.size() - MAX_HISTORY;
		this->_history_log.erase(_history_log.begin(),
				_history_log.begin() + (diff - 1));
	}
	this->_history_log.push_back(std::string(cmdString));
	return true;
}

int JobsList::BiggestJobNumSuspended()
{
	int ret_idx = ReturnValue::ERROR_VALUE;
	int listsize = int(this->_jobs.size());
	for (int i = listsize-1; i >= 0 ; --i)
	{
		if (this->_jobs[i].isStopped())
		{
			ret_idx = i;
			break;
		}
	}
	return ret_idx;			// if -1 than no sus job
}

/*****************************/
//----- Print Functions -----//
/*****************************/
std::string JobsList::toString(void)
{
	std::ostringstream oss;
	auto jobslist = this->GetJobs();
	for (size_t i = 0; i < jobslist.size(); ++i)
	{
		oss << "[" << (i + 1) << "] " << jobslist[i] << std::endl;
	} //end for
	return oss.str();
}

std::string JobsList::getSingalName(int signum)
{
	switch (signum)
	{
	case 1:
		return "SIGHUP";
	case 2:
		return "SIGINT";
	case 3:
		return "SIGQUIT";
	case 4:
		return "SIGILL";
	case 5:
		return "SIGTRAP";
	case 6:
		return "SIGABRT";
	case 7:
		return "SIGBUS";
	case 8:
		return "SIGFPE";
	case 9:
		return "SIGKILL";
	case 10:
		return "SIGUSR1";
	case 11:
		return "SIGSEGV";
	case 12:
		return "SIGUSR2";
	case 13:
		return "SIGPIPE";
	case 14:
		return "SIGALRM";
	case 15:
		return "SIGTERM";
	case 16:
		return "SIGSTKFLT";
	case 17:
		return "SIGCHLD";
	case 18:
		return "SIGCONT";
	case 19:
		return "SIGSTOP";
	case 20:
		return "SIGTSTP";
	case 21:
		return "SIGTTIN";
	case 22:
		return "SIGTTOU";
	case 23:
		return "SIGURG";
	case 24:
		return "SIGXCPU";
	case 25:
		return "SIGXFSZ";
	case 26:
		return "SIGVTALRM";
	case 27:
		return "SIGPROF";
	case 28:
		return "SIGWINCH";
	case 29:
		return "SIGIO";
	case 30:
		return "SIGPWR";
	case 31:
		return "SIGSYS";
	case 34:
		return "SIGRTMIN";
	default:
		return "No signal ID";
	}
}

