/*
 * jobprocess.cpp
 *
 *  Created on: Apr 9, 2020
 *      Author: os
 */

#include "jobprocess.h"


/*************************************************/
 //----- Class that defines a Job/Process -----//
/*************************************************/

job_process::job_process(std::string cmd, pid_t pid) :
		_cmd(cmd), _suspendTimer(-1), _pid(pid), _stopped(false)
{
	time(&this->_start);
}

/***************************/
//----- Get Functions -----//
/***************************/
time_t job_process::getRunningTime() const
{
	time_t current;
	time(&current);
	return difftime(current, this->_start);
}

time_t job_process::GetSuspendTime(void)
{
	time_t current;
	time(&current);
	return difftime(current, this->_suspendTimer);
}

pid_t job_process::getPid() const
{
	return _pid;
}

const std::string& job_process::getCmd() const
{
	return _cmd;
}

bool job_process::isStopped() const
{
	return _stopped;
}

/***************************/
//----- Set Functions -----//
/***************************/
void job_process::setStopped(bool stopped)
{
	_stopped = stopped;
	if(stopped)
	{
		time(&(this->_suspendTimer));
	}
	else
	{
		this->_suspendTimer = (time_t) (-1);
	}

}

/*****************************/
//----- Print Functions -----//
/*****************************/
std::ostream& operator<<(std::ostream &strm, const job_process &j)
{
	strm << j.getCmd() << " : " << j.getPid() << " " << j.getRunningTime()
			<< " secs";
	if(j.isStopped())
	{
		strm << " (Stopped)";
	}
	return strm;
}

