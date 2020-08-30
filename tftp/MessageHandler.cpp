/*
 * MessageHandler.cpp
 *
 *  Created on: Jun 15, 2020
 *      Author: os
 */

#include "MessageHandler.h"
#include <netinet/in.h>

/************************************/
//-- Constructor--//
/***********************************/
MessageHandler::MessageHandler()
{
}

//**************************************************************************************************************
// Function name: WRQ
// Description: Handle and returned write request query
// Parameters: a bytes buffer contained WRQ packet
//**************************************************************************************************************
WRQ_Pack MessageHandler::WRQ(char *buff)
{
	WRQ_Pack pack = {0};
	uint start, end;
	try
	{
		auto temp = (WRQ_Pack *)buff;
		if ((pack.opcode = ntohs(temp->opcode)) != MessageOpcode::WRQ)
		{
			pack.opcode = 0;
			return pack;
		}
		for (start = 0; (pack.File_name[start] = buff[OPCODE_SIZE + start]); start++)
			;
		for (end = 0; (pack.Trans_mode[end] = buff[OPCODE_SIZE + ++start]); end++)
			if (pack.Trans_mode[end])
				pack.Trans_mode[end] = std::tolower(pack.Trans_mode[end]);
		if (strcmp(pack.Trans_mode, OCTET))
		{
			pack.opcode = 0;
			return pack;
		}
	}
	catch (const std::exception &e)
	{
		error(e.what());
	}
	return pack;
}

//**************************************************************************************************************
// Function name: ACK
// Description: returned ACK
// Parameters: Ack number
//**************************************************************************************************************
ACK_Pack MessageHandler::ACK(int block_num)
{
	ACK_Pack pack;
	pack.opcode = htons(MessageOpcode::ACK);
	pack.BlockNumber = htons((unsigned short)block_num);
	return pack;
}

//**************************************************************************************************************
// Function name: Data
// Description: Handle and returned Data packet
// Parameters: a bytes buffer contained Data packet
//**************************************************************************************************************
Data_Pack MessageHandler::Data(char *buffer)

{
	Data_Pack return_message;
	try
	{
		auto pack = (Data_Pack *)buffer;
		pack->opcode = ntohs(pack->opcode);
		pack->BlockNumber = ntohs(pack->BlockNumber);
		if (pack->opcode != MessageOpcode::DATA) // opcode is incorrect
		{
			return_message.opcode = 0;
			return return_message;
		}
		return_message = *pack;
	}
	catch (const std::exception &e)
	{
		perror(e.what());
		return return_message;

	}

	return return_message;
}
