/*
 * Account2.h
 *
 *  Created on: May 28, 2020
 *      Author: os
 */

#ifndef ACCOUNT_H_
#define ACCOUNT_H_

#include <iostream>
#include <ctime>
#include <string>
#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <stdlib.h>

 //in order to create printable password 
#define DIGIT_SPLITER 10
enum PasswordParameters
{
	Size = 4, Min = 0, Max = 9999
};

// Constants for Read Write Locker
enum RWLocker
{
	MinToReader = 1, MaxToWrite = 0
};


/************************************/
 //-- Class that defined  Account--//
 //--Synchronized with RW locker --//
/***********************************/
class Account
{
public:
	Account(uint id, int password, unsigned long long balance);
	~Account();

	//----- Getters & Setters -----//
	unsigned long long GetBalance(void);
	void SetBalance(unsigned long long balance);
	uint GetId() const;
	int GetPassword() const;

	//----- Print Functions -----//
	friend std::ostream & operator<<(std::ostream &strm, Account &a);
	const std::string& GetPrintablePassword() const;
	static std::string PasswordToString(short password);


private:

	uint _ID;
	short _password;
	std::string _printablePassword;
	unsigned long long _balance;

	sem_t rd_lock;
	sem_t wrt_lock;
	int rcount = 0;

};

#endif /* ACCOUNT2_H_ */
