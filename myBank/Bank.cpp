/*
 * Bank.cpp
 *
 *  Created on: May 17, 2020
 *      Author: os
 */

#include "Bank.h"
#include <unistd.h>
#include <time.h>
Bank::Bank(AccountController * control) :
		_back_account(0), _stopCommission(false), _accountHandler(control)
{

}
void Bank::StopCommission()
{
	this->_stopCommission = true;
}

/*****************************/
//----- Print Thread -----//
/*****************************/

void Bank::PrintBalance()
{
	try
	{

		while (!this->_stopCommission)
		{
			this->_accountHandler->DownReadAction();
			printf("\033[2J");
			printf("\033[1;1H");
			std::cout << "Current Bank Status" << std::endl;
			std::cout << this->_accountHandler->GetAccountsBalance();
			std::cout << "Current Bank Status " << this->_back_account
					<< std::endl;
			this->_accountHandler->UpReadAction();
			usleep(BANK_PRINTER_INTERVAL);

		}
	} catch (std::exception& e){
		perror(e.what());
	}
	pthread_exit(NULL);

}

/*****************************/
//----- Commesion Thread -----//
/*****************************/
void Bank::TakeCommission()
{
	srand((unsigned) time(NULL));
	try
	{

		while (!this->_stopCommission)
		{
			int commission = (rand() % RandomCommission::Start)
					+ RandomCommission::End;
			this->_accountHandler->DownReadAction();
			auto amount = this->_accountHandler->TransferCommissions(
					commission);
			this->_accountHandler->UpReadAction();
			this->_back_account += amount;
			sleep(3);
		}
	} catch (std::exception& e)
	{
		perror(e.what());
	}
	pthread_exit(NULL);

}

