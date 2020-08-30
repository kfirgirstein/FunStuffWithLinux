/*
 * main.cpp
 *
 *  Created on: May 26, 2020
 *      Author: os
 */

#include "ATM.h"
#include "Bank.h"
#include "Logger.h"

AccountController * controller = new AccountController();

int main(int argc, char*argv[])
{
	if ((argc < 3) || (atoi(argv[1]) != (argc - 2)))
	{
		std::cerr
				<< "./Bank <Number of ATMs – N> <ATM_1_input_file> <ATM_2_input_file>…<ATM_N_input_file>"
				<< std::endl;
		return INVALID_VALUE;
	}

	int NumberOfATMs = atoi(argv[1]);
	pthread_t* ATM_threads = new pthread_t[NumberOfATMs];
	std::vector<ATM*> ATMList;
	//ATM ATMList [NumberOfATMs];

	pthread_t commission_thread, print_thread;
	Bank * bank = new Bank(controller);

	for (int i = 0; i < NumberOfATMs; i++) // start ATM threads
	{
		ATMList.push_back( new ATM(i+1,argv[i+2],controller));
		if (pthread_create(&(ATM_threads[i]), NULL,[](void * x)->void*{((ATM *)x)->Run();return NULL;},(void*)ATMList[i]))
		{
			perror("thread creation failed\n");
			exit(1);
		}
	}
	
	if (pthread_create(&commission_thread, NULL,
			[](void * x)->void*{((Bank *)x)->TakeCommission();return NULL;}, (void*)bank)) // start commission thread
	{
		perror("thread creation failed\n");
		exit(1);
	}
	if (pthread_create(&print_thread, NULL, [](void * x)->void*{((Bank *)x)->PrintBalance();return NULL;},(void*)bank)) // start print thread
	{
		perror("thread creation failed\n");
		exit(1);
	}

	for (int i = 0; i < NumberOfATMs; i++)
	{
		if (pthread_join(ATM_threads[i], NULL))
		{
			perror("thread join failed\n");
		}
	}
	
	bank->StopCommission();
	if (pthread_join(commission_thread, NULL)) // join commission thread when done
	{
		perror("thread join failed\n");
		exit(1);
	}
	if (pthread_join(print_thread, NULL)) // join print thread when done
	{
		perror("thread join failed\n");
		exit(1);
	}
	
	//AccountController::returnAccountController();
	delete[] ATM_threads;
	delete bank;
	for (int i = 0; i < NumberOfATMs; i++) // start ATM threads
	{
		delete ATMList[i];
	}
	delete controller;
	Logger::ReturnLogger();
	return 0;

}

