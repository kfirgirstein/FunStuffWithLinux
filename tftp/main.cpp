/*
 * main.cpp
 *
 *  Created on: May 26, 2020
 *      Author: os
 */

#include "tftpServer.h"

int main(int argc, char*argv[])
{
	if (argc != 2)
	{ //Check if program is correclty called
		std::cerr <<"Usage: ttftp [server_port]" <<std::endl;
	}

	int serverPort = atoi(argv[1]);
	if (serverPort <= 0 || serverPort > 65535)
	{ //Checking if the port given is valid and withing the accepted values
		std::cerr << "The port number given is wrong." <<std::endl;

	}
	try
	{
		tftpServer server(serverPort);
		server.Bind();
		server.Start();

	} catch (std::exception& e)
	{
		error(e.what());

	}
	return 0;

}

