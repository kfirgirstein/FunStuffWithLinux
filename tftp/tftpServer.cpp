/*
 * ttftpSrver.cpp
 *
 *  Created on: Jun 14, 2020
 *      Author: os
 */

#include "tftpServer.h"

//**************************************************************************************************************
// Function name: Constructor
// Description: create a new instance of tftp server
// Parameters: port number
//**************************************************************************************************************
tftpServer::tftpServer(unsigned short portno) : _portno(portno), _client_file(NULL)
{
	if (portno <= 0 || portno > 65535)
	{
		throw std::runtime_error("cannot use this port");
	}
	this->_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (this->_socket < 0)
	{
		internalError("TTFTP_ERROR: socket() failed\n", true);
	}
}
tftpServer::~tftpServer()
{
	close(this->_socket);
}

//**************************************************************************************************************
// Function name: Bind
// Description: activate the Bind function 0.0.0.0
// Parameters:  void
//**************************************************************************************************************
void tftpServer::Bind(void)
{
	//Preparing serverAddr
	memset(&this->_serverAddr, 0, sizeof(this->_serverAddr));
	this->_serverAddr.sin_family = AF_INET;
	this->_serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	this->_serverAddr.sin_port = htons(this->_portno);
	//Binding on the socket
	if (bind(this->_socket, (struct sockaddr *)&this->_serverAddr,
			 sizeof(this->_serverAddr)) < 0)
	{
		internalError("Cannot bind on UDP port", true);
	}
}

//**************************************************************************************************************
// Function name: Start
// Description: Connect a new client and recving file
// Parameters: void
//**************************************************************************************************************
void tftpServer::Start(void)
{
	unsigned int client_addr_len;
	struct sockaddr_in clientAddr;
	int bytes_read;
	FILE *fd;
	char buffer[BASIC_MSG] =
		{0};
	for (;;)
	{

		client_addr_len = sizeof(clientAddr);
		if ((bytes_read = recvfrom(this->_socket, buffer, BASIC_MSG, 0,
								   (struct sockaddr *)&clientAddr, &client_addr_len)) < 0)
		{
			// Got error on Recvfrom syscall, no file is here
			internalError("TTFTP_ERROR: recvfrom() failed\n", true);
		}
		auto request = this->_controller.WRQ(buffer);
		if (request.opcode != MessageOpcode::WRQ)
		{
			internalError("FLOWERROR:: received incorrect request query \n");
			continue;
		}
		std::cout << "IN:WRQ, " << request.File_name << ", "
				  << request.Trans_mode << std::endl;
		// send acknowledgment
		this->sendAck(this->_controller.ACK(0), (struct sockaddr *)&clientAddr, &client_addr_len);
		if ((fd = fopen(request.File_name, "w")) == NULL)
		{
			internalError("TTFTP_ERROR: fopen() failed\n", true);
		}
		this->_client_file = fd;
		this->_client_file_path = std::string(request.File_name);
		//read data
		if (this->readData(&clientAddr, &client_addr_len))
		{
			std::cout << "RECVOK" << std::endl;
			fclose(fd);
		}
		//file has written or RECVFAIL
		this->_client_file = NULL;
		this->_client_file_path.clear();
	}
}

//**************************************************************************************************************
// Function name: SendAck
// Description: Send ack pack to the client_addr in UDP
// Parameters: ACK packet, sockaddr pointer to client and its size
//**************************************************************************************************************
void tftpServer::sendAck(ACK_Pack pack, struct sockaddr *client_addr, socklen_t *client_addr_len)
{
	if (sendto(this->_socket, &pack, sizeof(pack), 0, client_addr, *client_addr_len) < 0)
	{
		// failed
		internalError("TTFTP_ERROR: sendto() failed\nRECVFAIL\n", true);
	}
	std::cout << "OUT:ACK, " << ntohs(pack.BlockNumber) << std::endl;
}

//**************************************************************************************************************
// Function name: readData
// Description: read the data file from the client to FIle, return True on success
// Parameters: sockaddr pointer to client its size and file
//**************************************************************************************************************
bool tftpServer::readData(struct sockaddr_in *client_addr, socklen_t *client_addr_len)
{
	const int WAIT_FOR_PACKET_TIMEOUT = 3;
	const int NUMBER_OF_FAILURES = 7;
	struct timeval timeout;
	fd_set readfds;
	int waiting_result = 0, Ack_counter = 0;
	long int bytes_read;
	unsigned int timeoutExpiredCount = 0;
	size_t lastWriteSize = 0;
	char buffer[MAX_MSG_WITH_HEADER] =
		{0};
	Data_Pack message;
	do
	{
		do
		{
			//Wait WAIT_FOR_PACKET_TIMEOUT to see if something appears
			timeout.tv_sec = WAIT_FOR_PACKET_TIMEOUT;
			timeout.tv_usec = 0;
			FD_ZERO(&readfds);
			FD_SET(this->_socket, &readfds);
			waiting_result = select(this->_socket + 1, &readfds, NULL, NULL,
									&timeout); // now we wait

			if (waiting_result > 0) //  if there was something at the socket and
			{
				bytes_read = recvfrom(this->_socket, &buffer,
									  sizeof(buffer), 0, (struct sockaddr *)client_addr, client_addr_len);
				if (bytes_read < 0)
				{
					internalError("TTFTP_ERROR: recvfrom() failed\nRECVFAIL\n", true);
				}
				message = this->_controller.Data(buffer);
				if (message.opcode != MessageOpcode::DATA)
				{
					internalError("FLOWERROR: received incorrect Data packet\nRECVFAIL\n");
					return false;
				}

				std::cout << "IN:DATA, " << message.BlockNumber << ", " << bytes_read << std::endl;
				if (message.BlockNumber != Ack_counter + 1) // block number incorrect
				{
					internalError("FLOWERROR: Data block number mismatch\nRECVFAIL\n");
					return false;
				}
			}

			if (0 == waiting_result)
			{
				// Time out expired while waiting for data
				// to appear at the socket
				timeoutExpiredCount++;
				//Send another ACK for the last packet
				this->sendAck(this->_controller.ACK(Ack_counter), (struct sockaddr *)client_addr, client_addr_len);
			}

			if (timeoutExpiredCount >= NUMBER_OF_FAILURES)
			{
				internalError("FLOWERROR: Receive data timeout\nRECVFAIL\n");
				return false;
			}

		} while (!waiting_result); // Continue while some socket was ready
		timeoutExpiredCount = 0;
		lastWriteSize = fwrite(message.Data, sizeof(char),
							   bytes_read - HEADER_SIZE, this->_client_file); // write next bulk of data

		std::cout << "Writing: " << lastWriteSize << std::endl;

		this->sendAck(this->_controller.ACK(++Ack_counter), (struct sockaddr *)client_addr, client_addr_len);
		timeoutExpiredCount = 0;
		// send ACK packet to the client
	} while (lastWriteSize == BASIC_MSG - 4); // Have blocks left to be read from client (not end of transmission)
	return true;
}

//**************************************************************************************************************
// Function name: readData
// Description: read the data file from the client to FIle
// Parameters: sockaddr pointer to client its size and file
//**************************************************************************************************************
void tftpServer::internalError(const char *msg, bool closeSocket)
{
	if (this->_client_file != NULL)
	{
		fclose(this->_client_file);
		remove(this->_client_file_path.c_str());
	}
	if (closeSocket && this->_socket > 0)
	{
		close(this->_socket);
		error(msg);
	}
	if (msg != NULL)
	{
		std::cerr << msg;
	}
}
