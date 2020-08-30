/*
 * AccountController.h
 *
 *  Created on: May 17, 2020
 *      Author: os
 */

#ifndef ACCOUNTCONTROLLER_H_
#define ACCOUNTCONTROLLER_H_
#include "Logger.h"
#include <vector>
#include "Account.h"

#define INVALID_VALUE -1
class AccountController
{
public:


	//----c'tor----//
	AccountController();

	//----d'tor----//
	~AccountController();

	/*****************************************/
	//-------- Transaction Functions --------//
	/*****************************************/
	bool AddAccount(uint id, int password, unsigned long long balance);
	bool AddToBalance(uint idx, uint amount);
	bool TakeFromBalance(uint idx, uint amount);
	unsigned long long GetAccountBalance(uint idx);
	std::string GetAccountsBalance(void);
	bool DeleteAccount(uint idx);
	unsigned long long TransferCommissions(double commissiom);

	/*******************************************/
	//------- Maintenance Functions -----------//
	/*******************************************/
	bool CheckPassword(uint id, int password);
	int GetIndexFromID(uint id);


	/*******************************************/
	//------- Synchronization Functions -------//
	/*******************************************/
	void DownReadAction();
	void UpReadAction();
	void DownWriteAction();
	void UpWriteAction();


private:

	 std::vector<Account> _accountList;
	 sem_t _db_rd_lock;
	 sem_t _db_wrt_lock;
	 int _db_rcount;
};

#endif /* ACCOUNTCONTROLLER_H_ */
