/*
 * MessageHandler.h
 *
 *  Created on: Jun 15, 2020
 *      Author: os
 */

#ifndef MESSAGEHANDLER_H_
#define MESSAGEHANDLER_H_

/************************************/
//-- Class that defined Message Handler--//
/***********************************/

#include "Messages.h"
class MessageHandler
{
public:
	MessageHandler();
	WRQ_Pack WRQ(char *buff);
	ACK_Pack ACK(int block_num);
	Data_Pack Data(char *buffer);
};

#endif /* MESSAGEHANDLER_H_ */
