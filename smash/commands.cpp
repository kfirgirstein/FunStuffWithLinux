//		commands.c
//********************************************
#include "commands.h"

/*********************************/
//----- Global Declarations -----//
/*********************************/
std::vector<std::string> history_log;
pid_t foreground_pid = ReturnValue::ERROR_VALUE;
std::string path_buffer;
bool is_path_buff_null = true;
auto jobsManager = JobsList::GetInstance();

/****************************/
//----- Parse Function -----//
/****************************/
int parseCommand(char* lineSize, char* args[])
{
	char* cmd;
	int num_arg = 0;
	const char* delimiters = " \t\n";
	cmd = strtok(lineSize, delimiters);
	if (cmd == NULL || args == NULL)
		return ReturnValue::ERROR_VALUE;
	args[0] = cmd;
	for (int i = 1; i < MAX_ARG; i++)
	{
		args[i] = strtok(NULL, delimiters);
		if (args[i] != NULL)
			num_arg++;
	}
	return num_arg;
}

/*******************************/
//----- Get Path Function -----//
/*******************************/
std::string get_path()
{
	char *wd(getcwd(NULL,0));
	std::string cwd(wd);
 	free(wd);
  	return cwd ;
}

//*************************************************************************************
// function name: SplitCommand
// Description: splitting command line by character into given vector
// Parameters: command string, delimiter char, vector reference
// Returns: the position of remainder substring, if exist
//**************************************************************************************
int SplitCommand(const std::string& s, char c, std::vector<std::string>& v)
{
	std::string::size_type i = 0;
	std::string::size_type j = s.find(c);
	if (j == std::string::npos)
	{
		return ReturnValue::Pass;
	}
	int remainder = ReturnValue::ERROR_VALUE;
	while (j != std::string::npos)
	{
		auto sub = s.substr(i, j - i);
		if (sub.size() > 0)
		{
			v.push_back(sub);
		}
		i = ++j;
		j = s.find(c, j);
		if (i == j) // doubled delimiter
		{
			v.clear();
			return ReturnValue::ERROR_VALUE;
		}
		if (j == std::string::npos)
		{
			remainder = i;
		}
	}
	return remainder;
}

//*************************************************************************************
// function name: ExeCmd
// Description: interprets and executes built-in commands
// Parameters: command string, line size
// Returns: 0 - success, 1 - failure
//**************************************************************************************
int ExeCmd(char* lineSize, char* cmdString)
{
	char* cmd;
	char pwd[MAX_LINE_SIZE];
	bool illegal_cmd = false; 	// illegal command
	char* args[MAX_ARG];
	int num_arg = parseCommand(lineSize, args);
	if (num_arg < 0 || args == NULL)
		return 0;
	cmd = args[0];

	/*************************************************/
	// Built in Commands
	/*************************************************/
	if (!strcmp(cmd, "cd"))
	{
		if (num_arg != 1)
		{
			illegal_cmd = true;
		}
		else
		{
			std::string temp = get_path();
			if (!strcmp(args[1], "-"))
			{
				if(is_path_buff_null)
					return ReturnValue::Pass;
				else if(path_buffer == temp)
				{
					//if trying to change to current directory
					std::cerr << "Error - trying to change to current directory" << std::endl;
					return ReturnValue::Pass;
				}
				else if(chdir(path_buffer.c_str()) == ReturnValue::ERROR_VALUE)
				{
					perror("Error Changing directory");
					return ReturnValue::Pass;
				}
				std::cout << path_buffer << std::endl;	// printing the updated current directory
				path_buffer = temp;						// then saving the former in the buffer

			} //end if
			else
			{
				std::string newDir = args[1];
				//if trying to change to current directory
				if (newDir == temp)
				{
					std::cerr << "Error - trying to change to current directory"
							<< std::endl;
					return ReturnValue::Pass;
				}
				if(chdir(newDir.c_str()) == ReturnValue::ERROR_VALUE)
				{
					perror("Error Changing directory");
					return ReturnValue::Pass;
				}
				path_buffer = temp;
				is_path_buff_null = false;
			}
		}
	}	//end cd command

	/*************************************************/

	else if (!strcmp(cmd, "cp"))
	{
		if (num_arg != 2)
		{
			illegal_cmd = true;
		}
		else
		{
			int old_file, new_file;
			ssize_t rd_cnt;
			ssize_t wr_cnt;

			//opening the files
			old_file = open(args[1], O_RDONLY);
			if (old_file == ReturnValue::ERROR_VALUE)
			{
				perror("Fail open old file for reading");
				return ReturnValue::Pass;
			}

			new_file = open(args[2], O_CREAT | O_WRONLY | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IRWXO);
			if (new_file == ReturnValue::ERROR_VALUE)
			{
				perror("Fail open new file for writing");
				if (close(old_file) == ReturnValue::ERROR_VALUE)
					perror("Fail closing old file");
				return ReturnValue::Pass;
			}

			//making sure they are not same
			if (!strcmp(args[1], args[2]))
			{
				std::cerr << "Error - copying file to itself" << std::endl;
				return ReturnValue::Pass;
			}

			//trying to copy the old file to the new file
			char files_buf;
			while ((rd_cnt = read(old_file, &files_buf, 1)) > 0)
			{
				wr_cnt = write(new_file, &files_buf, rd_cnt);	// rd_cnt = 1
				if (wr_cnt != rd_cnt)
				{
					perror("Fail writing to new file");
					if (close(new_file) || close(old_file))
						perror("Fail closing files");
					return ReturnValue::Pass;
				}
			}
			if (rd_cnt == ReturnValue::ERROR_VALUE) ////////////////////////////////////////////////
			{
				perror("Fail reading/writing file");
				return ReturnValue::Pass;
			}

			//trying to close the files
			if (close(new_file) || close(old_file))
			{
				perror("Fail closing files");
				return ReturnValue::Pass;
			}

			// printing successful copy
			std::cout << args[1] << " had been copied to " << args[2]
					<< std::endl;
		}
	}	//end cp command

	/*************************************************/

	else if (!strcmp(cmd, "diff"))
	{

		if (num_arg != 2)
		{
			illegal_cmd = true;
		}
		else
		{
			bool files_identical = true;
			int f1, f2;
			ssize_t rd1_cnt, rd2_cnt;
			char file1_buffer, file2_buffer;

			//opening the files
			f1 = open(args[1], O_RDONLY);
			if (f1 == ReturnValue::ERROR_VALUE)
			{
				perror("Fail open file NO.1");// maybe add (illegal_cmd = true) at all these places...
				return ReturnValue::Pass;
			}

			else if ((f2 = open(args[2], O_RDONLY)) == ReturnValue::ERROR_VALUE)
			{
				perror("Fail open file NO.2");
				if (close(f1))
					perror("Fail closing file NO.1");
				return ReturnValue::Pass;
			}

			//making sure they are not same
			if (!strcmp(args[1], args[2]))
			{
				std::cerr << "Error - comparing file to itself" << std::endl;
				return ReturnValue::Pass;
			}

			//trying to compare the files
			else
			{
				do
				{
					rd1_cnt = read(f1, &file1_buffer, 1);
					rd2_cnt = read(f2, &file2_buffer, 1);

					if (rd1_cnt != rd2_cnt)
					{
						files_identical = false;
						break;
					}
					if (memcmp(&file1_buffer, &file2_buffer, rd1_cnt))
					{
						files_identical = false;
						break;
					}

				} while (rd1_cnt > 0);

				// check if files equal only for f1 length
				if((files_identical) && ((rd2_cnt = read(f2, &file2_buffer, 1)) > 0 ))
				{
					files_identical = false;
				}

				//trying to close the files
				if (close(f1) || close(f2))
				{
					perror("Fail closing files");
					return ReturnValue::Pass;
				}

				// printing successful compare
				std::cout << ((int) !files_identical) << std::endl;
			}
		}
	}	//end diff command

	/*************************************************/

	else if (!strcmp(cmd, "pwd"))
	{
		if (num_arg != 0)
		{
			illegal_cmd = true;
		}
		else
		{
			if (getcwd(pwd, MAX_LINE_SIZE) == NULL)
			{
				perror("Error getting the working directory");
				return ReturnValue::Pass;
			}
			std::cout << pwd << std::endl;
		}	//end else

	}	//end pwd command

	/*********************************************/

	else if (!strcmp(cmd, "jobs"))
	{
		if (num_arg != 0)
		{
			illegal_cmd = true;
		}
		else
		{
			std::cout << jobsManager->toString();
		}
	}	//end jobs command

	/*************************************************/

	else if (!strcmp(cmd, "showpid"))
	{
		if (num_arg != 0)
		{
			illegal_cmd = true;
		}
		else
		{
			std::cout << "smash pid is " << getpid() << std::endl;
		}
	}	//end showpid command

	/*************************************************/

	else if (!strcmp(cmd, "fg"))
	{
		uint jobs_size = jobsManager->GetJobs().size();
		uint job_idx;
		if (num_arg == 0)
		{
			job_idx = jobs_size;

			// if there are no jobs to be foreground
			if (job_idx <= 0)
			{
				std::cout << "Error - There are no jobs at all" << std::endl;
				return ReturnValue::Pass;
			}

			// prints the foreground job and waits
			std::cout << jobsManager->GetJobs()[job_idx - 1].getCmd()
					<< std::endl;
			jobsManager->WaitJob(job_idx - 1);
		}
		else
		{
			try
			{
				// taking args - exception will be thrown for illegal args
				job_idx = std::stoi(args[1]);
				if ((num_arg > 1) || (job_idx <= 0) || (jobs_size < job_idx))
				{
					illegal_cmd = true;
				}

			} catch (...)
			{
				illegal_cmd = true;
			}
			if (!illegal_cmd) //no exception was made
			{
				// prints the foreground job and waits
				std::cout << jobsManager->GetJobs()[job_idx - 1].getCmd()
						<< std::endl;
				jobsManager->WaitJob(job_idx - 1);
			}
		}

	}		//end fg command

	/*************************************************/

	else if (!strcmp(cmd, "bg"))
	{
		uint jobs_size = jobsManager->GetJobs().size();
		if (num_arg == 0)
		{
			int job_idx = jobsManager->BiggestJobNumSuspended();
			if (job_idx == ReturnValue::ERROR_VALUE
					|| false == jobsManager->WakeJob(job_idx))
			{
				// if there are no suspended jobs to send back to work
				if (job_idx == ReturnValue::ERROR_VALUE)
					std::cout << "Error - There are no suspended jobs" << std::endl;
				return ReturnValue::Pass;

			}
			// prints the background job
			std::cout << jobsManager->GetJobs()[job_idx].getCmd() << std::endl;
		}
		else
		{
			uint job_idx;
			try
			{
				// taking args - exception will be thrown for illegal args
				job_idx = std::stoi(args[1]);
				if ((num_arg > 1) || (job_idx <= 0) || (jobs_size < job_idx))
				{
					illegal_cmd = true;
				}

			} catch (...)
			{
				illegal_cmd = true;
			}
			if (!illegal_cmd) //no exception was made
			{
				if (!jobsManager->GetJobs()[job_idx - 1].isStopped()
						|| false == jobsManager->WakeJob(job_idx - 1))
				{
					std::cout << "Error - This job was not suspended"
							<< std::endl;
					return ReturnValue::Pass;
				}

				// prints the background job
				std::cout << jobsManager->GetJobs()[job_idx - 1].getCmd()
						<< std::endl;
			}

		}

	}		//end bg command

	/*************************************************/

	else if (!strcmp(cmd, "history"))
	{
		if (num_arg != 0)
		{
			illegal_cmd = true;
		}
		else
		{
			for (auto it : jobsManager->GetHistory())
			{
				std::cout << it << std::endl;
			}
		}
	}		//end history command

	/*************************************************/

	else if (!strcmp(cmd, "kill"))
	{
		if ((num_arg != 2) || ((args[1])[0] != '-'))
		{
			illegal_cmd = true;
		}
		else
		{
			uint jobs_size = jobsManager->GetJobs().size();
			uint job_idx;
			int signal;
			try
			{
				// taking args - exception will be thrown for illegal args
				job_idx = std::stoi(args[2]);
				if ((job_idx <= 0) || (jobs_size < job_idx))
				{
					fprintf(stderr, "kill %d - job does not exist\n", job_idx);
					return ReturnValue::Pass;
				}
				signal = std::stoi(strtok(args[1], "-"));
			} catch (...)
			{
				illegal_cmd = true;
			}
			if(!illegal_cmd)
			{
				jobsManager->KillJob(job_idx - 1, signal);
			}
		}

	}		//end kill command

	/*************************************************/

	else if (!strcmp(cmd, "quit"))
	{
		if ((num_arg > 1) || ((num_arg == 1) && (strcmp(args[1], "kill"))))
		{
			illegal_cmd = true;
		}
		else
		{
			signal(SIGCHLD, SIG_DFL);
			if (num_arg == 1)
			{
				jobsManager->KillAllJobs();
			}
			JobsList::RemoveInstance();
			signal(SIGINT, SIG_DFL);
			signal(SIGTSTP, SIG_DFL);
			exit(0);
		}
	}		//end quit command

	/*************************************************/

	else // external command
	{
		ExeExternal(args, cmdString);
	}

	///********************  here ends all the command cases  ***********************///

	if (illegal_cmd)
	{
		// printing general message for illegal arguments
		printf("smash error: > \"%s\"\n", cmdString);
		return 1;
	}
	return 0;
}

//**************************************************************************************
// function name: ExeExternal
// Description: executes external command
// Parameters: external command arguments, external command string
// Returns: void
//**************************************************************************************
void ExeExternal(char *args[MAX_ARG], char* cmdString, bool background)
{
	pid_t pID;
	int wait_stauts;
	switch (pID = fork())
	{
	case ReturnValue::ERROR_VALUE:
		std::cerr << "Error in fork function" << std::endl;
		return;
	case ReturnValue::Pass:
		setpgrp(); // set group ID
		if (execvp(args[0], args) == -1) // try to run external command
		{ //here signals are back to SIG_DFL
			fprintf(stderr, "Failure running external command: %s - \n", args[0]);
			if (kill(getpid(), SIGKILL)) // kill if unsuccessful
				std::cerr << "SIGKILL - cannot send signal" << std::endl;
		}
		break;

	default:
		// dad process
		if (background)
		{
			auto j = job_process(cmdString, pID);
			jobsManager->AddJob(j);
			std::cout << "[" << jobsManager->GetJobs().size() << "] "
					<< j.getPid() << std::endl;
		}
		else
		{
			//foreground_pid = pID;
			wait_stauts = JobsList::WaitToProcess(pID);
			if (WIFSTOPPED(wait_stauts))
			{
				auto j = job_process(cmdString, pID);
				j.setStopped(true);
				jobsManager->AddJob(j);
			}
		}
		//wait(1);
		break;
	}

}

//**************************************************************************************
// function name: BgCmd
// Description: if command is in background, insert the command to jobs
// Parameters: command string, pointer to jobs
// Returns: 0- BG command -1- if not
//**************************************************************************************
int BgCmd(char* lineSize)
{

	char delimiters = '&';
	char *args[MAX_ARG];
	std::vector<std::string> commands;

	auto rest = SplitCommand(lineSize, delimiters, commands);
	switch (rest)
	{
	case ReturnValue::Pass:
		return ReturnValue::Pass;
	case ReturnValue::ERROR_VALUE:
		std::cout << "smash error> syntax error near unexpected token `&'"
				<< std::endl;
		return ReturnValue::ERROR_VALUE;
	default:
	{
		for (auto &cmd : commands)
		{
			if (parseCommand((char*) cmd.data(), args) < 0)
			{
				break;
			}
			if (std::find(COMMANDS.begin(), COMMANDS.end(), args[0])
					== COMMANDS.end()) //if this is not built in command
			{
				ExeExternal(args, (char*) cmd.data(), true);
			}
			else
			{
				std::cout << "smash error: > cannot run built in in background"
						<< std::endl;
			}
		}
		ExeCmd(lineSize + rest, lineSize);
	}
	}
	return ReturnValue::Success;
}

