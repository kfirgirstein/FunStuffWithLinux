// signals.c
// contains signal handler funtions
// contains the function/s that set the signal handlers

/*******************************************/
// Name: handler_cntlc
// Synopsis: handle the Control-C
/*******************************************/

#include "signals.h"

void SIGINT_handler(int signum)
{
	if (foreground_pid != ReturnValue::ERROR_VALUE)
	{
		std::cout << std::endl;
		if (JobsList::SendSignalToProcess(foreground_pid, signum))
		{
			foreground_pid = ReturnValue::ERROR_VALUE;
		}
		else
		{
			std::cerr << "CTRL+C failed" << std::endl;
		}
	}

}
void SIGTSTP_handler(int signum)
{
	if (foreground_pid != ReturnValue::ERROR_VALUE)
	{
		std::cout << std::endl;
		if (JobsList::SendSignalToProcess(foreground_pid, signum))
		{
			foreground_pid = ReturnValue::ERROR_VALUE;
		}
		else
		{
			std::cerr << "CTRL+Z failed" << std::endl;
		}
	}
}
void SIGCHILD_handler(int signum)
{
	JobsList::GetInstance()->CleanFinished();
}
