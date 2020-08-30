/*
 * ATM.cpp
 *
 *  Created on: May 17, 2020
 *      Author: os
 */

#include "ATM.h"
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>

//----c'tor----//
ATM::ATM(uint ID, const char * fileName, AccountController * accountHandler) :
		_ID(ID), _fileName(fileName), _accountHandler(accountHandler) {}


//---- make transactions by input lines ----//
bool ATM::HandleLine(char * line)
{
	std::vector<std::string> splited;

	std::istringstream iss(line);
	for (std::string s; iss >> s;)
		splited.push_back(s);

	if (splited.size() < 3)
	{
		return false;
	}
	char parameter = splited[0][0];
	auto account = atoi(splited[1].c_str());
	auto password = atoi(splited[2].c_str());
	if (password < PasswordParameters::Min || password > PasswordParameters::Max
			|| account < 0)
	{
		LOG->write("Error " + std::to_string(this->_ID) + ":Invalid argument(s)");
		return false;
	}
	switch (parameter)
	{
	case 'O':
	{
		if (splited.size() != 4)
		{
			LOG->write("Error " + std::to_string(this->_ID) + ":Incorrect number of arguments");
			return false;
		}
		OpenAccount(account, password, atoi(splited[3].c_str()));
	}
	break;
	case 'D':
	{
		if (splited.size() != 4)
		{
			LOG->write("Error " + std::to_string(this->_ID) + ":Incorrect number of arguments");
			return false;
		}
		DepositToAccount(account, password, atoi(splited[3].c_str()));
	}
	break;
	case 'W':
	{
		if (splited.size() != 4)
		{
			LOG->write("Error " + std::to_string(this->_ID) + ":Incorrect number of arguments");
			return false;
		}
		WithdrawToAccount(account, password, atoi(splited[3].c_str()));
	}
	break;
	case 'B':
	{
		if (splited.size() != 3)
		{
			LOG->write("Error " + std::to_string(this->_ID) + ":Incorrect number of arguments");
			return false;
		}
		BalanceInquiry(account, password);
	}
	break;
	case 'T':
	{
		if (splited.size() != 5)
		{
			LOG->write("Error " + std::to_string(this->_ID) + ":Incorrect number of arguments");
			return false;
		}
		Transfer(account, password, atoi(splited[3].c_str()),
				atoi(splited[4].c_str()));
	}
	break;
	case 'Q':
	{
		if (splited.size() != 3)
		{
			LOG->write("Error " + std::to_string(this->_ID) + ":Incorrect number of arguments");
			return false;
		}
		CloseAccount(account, password);
	}
	break;
	default:
	LOG->write("Error " + std::to_string(this->_ID) + ": Invalid command");
	break;
	}
	return true;

}


//----Run func for Atm-Thread----//
void ATM::Run(void)
{
	std::ifstream _ATMFile(this->_fileName);
	if (!_ATMFile.is_open())
	{
		perror("Error opening file in ATM");
		pthread_exit(NULL);
	}
	std::string line;
	try
	{
		while (std::getline(_ATMFile, line))
		{
			HandleLine(const_cast<char*>(line.c_str()));
			usleep(ATM_ACTION_INTERVAL);
		}
	} catch (std::exception& e)
	{
		perror(e.what());
	}
	_ATMFile.close();
	pthread_exit(NULL);

}


/*****************************************/
//-------- Transaction Functions --------//
/*****************************************/
bool ATM::OpenAccount(uint id, int password, long long balance)
{
	if (balance < 0)
	{
		LOG->write("Error " + std::to_string(this->_ID) + ":Invalid argument(s)");
		return false;
	}
	std::ostringstream oss;
	this->_accountHandler->DownWriteAction();
	if (!this->_accountHandler->AddAccount(id, password, balance))
	{
		this->_accountHandler->UpWriteAction();
		oss << "Error " << this->_ID
		<< ": Your transaction failed - account with the same id exists";
		LOG->write(oss.str());
		this->_accountHandler->UpWriteAction();
		return false;
	}
	sleep(1);
	this->_accountHandler->UpWriteAction();
	oss << "" << this->_ID << ": New account id is " << id << " with password "
	<< Account::PasswordToString(password) << " and initial balance "
	<< balance;
	LOG->write(oss.str());
	return true;
}

bool ATM::DepositToAccount(uint id, int password, long long amount)
{
	if (amount < 0)
	{
		LOG->write("Error " + std::to_string(this->_ID) + ":Invalid argument(s)");
		return false;
	}
	std::ostringstream oss;
	this->_accountHandler->DownReadAction();
	if (!ValidatedPassword(id, password))
	{
		this->_accountHandler->UpReadAction();
		return false;
	}
	int idx = this->_accountHandler->GetIndexFromID(id);
	this->_accountHandler->AddToBalance(idx, amount);
	sleep(1);
	auto bal = this->_accountHandler->GetAccountBalance(idx);
	this->_accountHandler->UpReadAction();
	oss << "" << this->_ID << ": Account " << id << " new balance is " << bal
	<< " after " << amount << " $ was deposited";
	LOG->write(oss.str());
	return true;
}

bool ATM::WithdrawToAccount(uint id, int password, long long amount)
{
	if (amount < 0)
	{
		LOG->write("Error " + std::to_string(this->_ID) + ":Invalid argument(s)");
		return false;
	}
	std::ostringstream oss;
	this->_accountHandler->DownReadAction();
	if (!ValidatedPassword(id, password))
	{
		this->_accountHandler->UpReadAction();
		return false;
	}
	int idx = this->_accountHandler->GetIndexFromID(id);
	if (!this->_accountHandler->TakeFromBalance(idx, amount))
	{
		this->_accountHandler->UpReadAction();
		oss << "Error " << this->_ID << ": Your transaction failed - account id "
		<< id << " balance is lower than " << amount;
		LOG->write(oss.str());
		return false;
	}
	auto bal = this->_accountHandler->GetAccountBalance(idx);
	sleep(1);
	this->_accountHandler->UpReadAction();
	oss << "" << this->_ID << ": Account " << id << " new balance is " << bal
	<< " after " << amount << " $ was withdrew";
	LOG->write(oss.str());
	return true;
}

bool ATM::BalanceInquiry(uint id, int password)
{
	std::ostringstream oss;
	this->_accountHandler->DownReadAction();
	int idx = this->_accountHandler->GetIndexFromID(id);
	if (!ValidatedPassword(id, password))
	{
		this->_accountHandler->UpReadAction();
		return false;
	}
	auto bal = this->_accountHandler->GetAccountBalance(idx);
	sleep(1);
	this->_accountHandler->UpReadAction();
	oss << "" << this->_ID << ": Account " << id << " balance is " << bal;
	LOG->write(oss.str());
	return true;
}

bool ATM::CloseAccount(uint id, int password)
{
	std::ostringstream oss;
	this->_accountHandler->DownWriteAction();
	int idx = this->_accountHandler->GetIndexFromID(id);
	if (!ValidatedPassword(id, password))
	{
		this->_accountHandler->UpWriteAction();
		return false;
	}
	auto bal = this->_accountHandler->GetAccountBalance(idx);
	this->_accountHandler->DeleteAccount(idx);
	this->_accountHandler->UpWriteAction();
	oss << "" << this->_ID << ": Account " << id
			<< " is now closed. balance was " << bal;
	LOG->write(oss.str());
	return true;
}

bool ATM::Transfer(uint from_ID, int from_password, uint target_ID,
		long long amount)
{
	if (amount < 0)
	{
		LOG->write("Error " + std::to_string(this->_ID) + ":Invalid argument(s)");
		return false;
	}
	std::ostringstream oss;
	this->_accountHandler->DownWriteAction();
	int from_idx = this->_accountHandler->GetIndexFromID(from_ID);
	int to_idx = this->_accountHandler->GetIndexFromID(target_ID);
	if (from_idx == INVALID_VALUE) // if this account id already exists
	{
		this->_accountHandler->UpWriteAction();
		oss << "Error " << this->_ID
		<< ": Your transaction failed - account id " << from_ID
		<< " does not exist";
		LOG->write(oss.str());
		return false;
	}
	if (to_idx == INVALID_VALUE) // if this account id already exists
	{
		this->_accountHandler->UpWriteAction();
		oss << "Error " << this->_ID
		<< ": Your transaction failed - account id " << target_ID
		<< " does not exist";
		return false;
	}
	if (!this->_accountHandler->CheckPassword(from_idx, from_password))
	{
		this->_accountHandler->UpWriteAction();
		oss << "Error " << this->_ID
		<< ": Your transaction failed - password for account id "
		<< from_ID << " is incorrect";
		LOG->write(oss.str());
		return false;
	}
	if (!this->_accountHandler->TakeFromBalance(from_idx, amount))
	{
		this->_accountHandler->UpWriteAction();
		oss << "Error " << this->_ID
		<< ": Your transaction failed - account id " << from_ID
		<< " balance is lower than " << amount;
		LOG->write(oss.str());
		return false;
	}
	this->_accountHandler->AddToBalance(to_idx, amount);
	sleep(1);
	auto from_bal = this->_accountHandler->GetAccountBalance(from_idx);
	auto to_bal = this->_accountHandler->GetAccountBalance(to_idx);
	this->_accountHandler->UpWriteAction();
	oss << "" << this->_ID << ": Transfer " << amount << " from accounts "
	<< from_ID << " to account " << target_ID
	<< " new account balance is " << from_bal
	<< " new target account balance is " << to_bal;
	LOG->write(oss.str());
	return true;
}

//---- internal args check ----//
bool ATM::ValidatedPassword(uint id, int password)
{
	std::ostringstream oss;
	int idx = this->_accountHandler->GetIndexFromID(id);
	if (idx == INVALID_VALUE) // if this account id already exists
	{
		oss << "Error " << this->_ID
				<< ": Your transaction failed - account id " << id
				<< " does not exist";
		LOG->write(oss.str());
		return false;
	}
	if (!this->_accountHandler->CheckPassword(idx, password))
	{
		oss << "Error " << this->_ID
				<< ": Your transaction failed - password for account id " << id
				<< " is incorrect";
		LOG->write(oss.str());
		return false;
	}
	return true;
}

