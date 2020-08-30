/*
 * Account.cpp
 *
 *  Created on: May 17, 2020
 *      Author: os
 */
#include "Account.h"

Account::Account(uint id, int password, unsigned long long init_balance) :
		_ID(id), _password(PasswordParameters::Min), _balance(0)
{
	if(password<PasswordParameters::Min || password>PasswordParameters::Max)
	{
		throw std::out_of_range("Password out of range!");
	}
	if (sem_init(&this->rd_lock, 0, RWLocker::MinToReader) != 0)
	{
		std::cerr << "Write semaphore initialization  has failed" << std::endl;
	}
	if (sem_init(&this->wrt_lock, 0, RWLocker::MinToReader) != 0)
	{
		std::cerr << "Read semaphore initialization has failed" << std::endl;
	}
	this->_password = password;
	this->_printablePassword = PasswordToString(password);
	this->SetBalance(init_balance);
}

Account::~Account()
{
	sem_destroy(&this->wrt_lock);
	sem_destroy(&this->rd_lock);
}

/*****************************/
//----- Getters & Setters -----//
/*****************************/
uint Account::GetId() const
{
	return _ID;
}

int Account::GetPassword() const
{
	return _password;
}

/*********************************************/
//----- Write action to  balance ------------//
//Synchronization with Write lock (semaphore)//
/********************************************/
void Account::SetBalance(unsigned long long balance)
{
	sem_wait(&this->wrt_lock);
	_balance = balance;
	sem_post(&this->wrt_lock);
}

/*********************************************/
//----- Read action to  balance ------------//
//Synchronization with Read lock (semaphore)//
/********************************************/
unsigned long long  Account::GetBalance(void)
{
	sem_wait(&this->rd_lock);
	this->rcount++;
	if (this->rcount == RWLocker::MinToReader)
	{
		sem_wait(&this->wrt_lock);
	}
	sem_post(&this->rd_lock);
	uint bal = this->_balance;
	sem_wait(&this->rd_lock);
	this->rcount--;
	if (this->rcount == RWLocker::MaxToWrite)
	{
		sem_post(&this->wrt_lock);

	}
	sem_post(&this->rd_lock);

	return bal;
}


/*****************************/
//----- Print Functions -----//
/*****************************/
const std::string& Account::GetPrintablePassword() const
{
		return this->_printablePassword;
}

std::ostream& operator<<(std::ostream &strm, Account &a)
{
	strm << "Balance - " << a.GetBalance() << " $ , Account Password - "
			<< a.GetPrintablePassword();
	return strm;
}

std::string Account::PasswordToString(short password)
{
	std::string printed = "";
	for (int i = 0; i < PasswordParameters::Size; ++i, password /= DIGIT_SPLITER)
	{
		printed = (char) ('0' + password % DIGIT_SPLITER)
				+ printed;
	}
	return printed;
}
