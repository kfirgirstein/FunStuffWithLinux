/*
 * ttftpSrver.h
 *
 *  Created on: Jun 14, 2020
 *      Author: os
 */

#ifndef TFTPSERVER_H_
#define TFTPSERVER_H_

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/select.h>
#include <unistd.h>
#include <netdb.h>

#include "MessageHandler.h"

/************************************/
//-- Class that defined tftpServer --//
/***********************************/
class tftpServer
{
public:
	tftpServer(unsigned short portno);
	~tftpServer();
	void Bind(void);
	void Start(void);

private:
	/**** Client Handle Functions ****/
	inline void sendAck(ACK_Pack pack, struct sockaddr *client_addr, socklen_t *client_addr_len);
	bool readData(struct sockaddr_in *client_addr, socklen_t *client_addr_len);
	inline void internalError(const char *msg, bool closeSocket = false);

	/** Attributes **/
	int _socket;
	unsigned short _portno;
	struct sockaddr_in _serverAddr;
	MessageHandler _controller;

	FILE *_client_file;
	std::string _client_file_path;
};

#endif /* TFTPSERVER_H_ */
