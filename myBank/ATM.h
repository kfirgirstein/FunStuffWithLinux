/*
 * ATM.h
 *
 *  Created on: May 17, 2020
 *      Author: os
 */

#ifndef ATM_H_
#define ATM_H_
#include "Logger.h"
#include "AccountController.h"
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <unistd.h>


#define ATM_ACTION_INTERVAL 100000


class ATM
{
public:
	//----c'tor----//
	ATM(uint ID,const char * fileName,AccountController * _accountHandler);

	//---- make transactions by input lines ----//
	bool HandleLine(char * line);

	//----Run func for Atm-Thread----//
	void Run(void);

	/*****************************************/
	//-------- Transaction Functions --------//
	/*****************************************/
	bool OpenAccount(uint id, int password, long long balance);
	bool DepositToAccount(uint id, int password, long long amount);
	bool WithdrawToAccount(uint id, int password, long long amount);
	bool BalanceInquiry(uint id, int password);
	bool CloseAccount(uint id, int password);
	bool Transfer(uint from_ID, int from_password, uint target_ID, long long amount);



private:

	uint _ID;
	const char * _fileName;

	//---- vector of all accounts ----//
	AccountController * _accountHandler;

	//---- internal args check ----//
	bool ValidatedPassword(uint id, int password);

};

#endif /* ATM_H_ */
