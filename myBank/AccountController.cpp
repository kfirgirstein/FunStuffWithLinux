/*
 * AccountController.cpp
 *
 *  Created on: May 17, 2020
 *      Author: os
 */

#include <sstream>
#include "AccountController.h"

//----c'tor----//
AccountController::AccountController()
{
	if (sem_init(&(_db_wrt_lock), 0, RWLocker::MinToReader) != 0)
	{
		std::cerr << "Write semaphore initialization  has failed" << std::endl;
	}
	if (sem_init(&(_db_rd_lock), 0, RWLocker::MinToReader) != 0)
	{
		std::cerr << "Read semaphore initialization has failed" << std::endl;
	}
}

//----d'tor----//
AccountController::~AccountController()
{
	sem_destroy(&(_db_wrt_lock));
	sem_destroy(&(_db_rd_lock));
}


/*****************************************/
//-------- Transaction Functions --------//
/*****************************************/

bool AccountController::AddAccount(uint id, int password,
		unsigned long long balance)
{
	try
	{
		if (GetIndexFromID(id) != INVALID_VALUE)
		{
			return false;
		}
		_accountList.push_back(Account(id, password, balance));
	} catch (std::exception& e)
	{
		perror(e.what());
		return false;
	}
	return true;
}

bool AccountController::AddToBalance(uint idx, uint amount)
{
	try
	{
		_accountList[idx].SetBalance(_accountList[idx].GetBalance() + amount);
	} catch (std::exception& e)

	{
		perror(e.what());
		return false;
	}
	return true;
}

bool AccountController::TakeFromBalance(uint idx, uint amount)
{
	if (_accountList[idx].GetBalance() < amount)
		return false;
	_accountList[idx].SetBalance(_accountList[idx].GetBalance() - amount);
	return true;
}

unsigned long long AccountController::GetAccountBalance(uint idx)
{
	return _accountList[idx].GetBalance();
}


bool AccountController::DeleteAccount(uint idx)
{
	if (_accountList.end() == _accountList.erase(_accountList.begin() + idx))
	{
		return false;
	}
	return true;
}

unsigned long long AccountController::TransferCommissions(double commissiom)
{
	unsigned long long tot_amount = 0;
	int private_comission = 0;
	double com_precentage = commissiom / 100;
	for (size_t index = 0; index < _accountList.size(); ++index)
	{
		private_comission = __uint64_t(
				com_precentage * ((double) (_accountList[index].GetBalance()))); // maybe need better Igol than the int... need to be to the closest Z number
		if (!TakeFromBalance(index, private_comission))
			return false;
		tot_amount += private_comission;
	}
	return tot_amount;
}

std::string AccountController::GetAccountsBalance(void)
{
	std::ostringstream oss;
	if (!_accountList.empty())
	{
		auto members = _accountList;
		for (size_t i = 0; i < members.size(); ++i)
		{
			oss << "Account " << (i + 1) << ": " << members[i] << std::endl;
		} //end for
	}
	return oss.str();
}


/*******************************************/
//------- Maintenance Functions -----------//
/*******************************************/

bool AccountController::CheckPassword(uint idx, int password)
{
	if (password != _accountList[idx].GetPassword())
	{
		return false;
	}
	return true;
}

int AccountController::GetIndexFromID(uint id)
{
	if (!_accountList.empty())
	{
		for (size_t index = 0; index < _accountList.size(); ++index)
		{
			if (_accountList[index].GetId() == id)
				return index;
		}
	}
	return INVALID_VALUE;
}


/*******************************************/
//------- Synchronization Functions -------//
/*******************************************/

void AccountController::DownReadAction()
{
	sem_wait(&(_db_rd_lock));
	_db_rcount++;
	if (_db_rcount == RWLocker::MinToReader)
	{
		sem_wait(&(_db_wrt_lock));
	}
	sem_post(&(_db_rd_lock));
}

void AccountController::UpReadAction()
{
	sem_wait(&(_db_rd_lock));
	_db_rcount--;
	if (_db_rcount == RWLocker::MaxToWrite)
	{
		sem_post(&(_db_wrt_lock));
	}
	sem_post(&(_db_rd_lock));
}

void AccountController::DownWriteAction()
{
	sem_wait(&(_db_wrt_lock));
}

void AccountController::UpWriteAction()
{
	sem_post(&(_db_wrt_lock));
}
