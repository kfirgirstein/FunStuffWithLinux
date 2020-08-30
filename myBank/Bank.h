/*
 * Bank.h
 *
 *  Created on: May 17, 2020
 *      Author: os
 */

#ifndef BANK_H_
#define BANK_H_
#include <cstdlib>
#include <ctime>
#include <iostream>
#include "AccountController.h"

//print Interval Constance
#define BANK_PRINTER_INTERVAL 500000 
// Rang for commission 
enum RandomCommission{Start=3,End=2};

/*************************************************/
 //-- Class that defines a Bank (and his Threads) --//
/*************************************************/
class Bank
{
public:
	Bank(AccountController *);
	void StopCommission();

	//----- Print Thread -----//
	void PrintBalance();
	//----- Commesion Thread -----//
	void TakeCommission();
	


private:
	unsigned long long _back_account;
	bool _stopCommission; // validation whether the ATMs has ended 
	AccountController * _accountHandler;

};

#endif /* BANK_H_ */
