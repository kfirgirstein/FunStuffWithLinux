/*
 * jobprocess.h
 *
 *  Created on: Apr 9, 2020
 *      Author: os
 */

#ifndef JOBPROCESS_H_
#define JOBPROCESS_H_
#include<ctime>
#include<string>
#include <iostream>


/*************************************************/
 //----- Class that defines a Job/Process -----//
/*************************************************/

class job_process
{
public:
	job_process(std::string cmd, pid_t pid);

	 //----- Get Functions -----//
	time_t getRunningTime() const;
	time_t GetSuspendTime();
	pid_t getPid() const;
	const std::string& getCmd() const;
	bool isStopped() const;

	 //----- Set Functions -----//
	void setStopped(bool stopped);

	 //----- Print Functions -----//
	friend std::ostream & operator<<(std::ostream &strm, const job_process &j);

private:
	std::string _cmd;
	time_t _suspendTimer;
	time_t _start; 				// when the job entered container
	pid_t _pid;
	bool _stopped; 				//if the job is suspended

};

#endif /* JOBPROCESS_H_ */
