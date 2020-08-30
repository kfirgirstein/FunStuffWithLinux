#define BASIC_MSG 516
#define OPCODE_SIZE 2
#define HEADER_SIZE 4
#define MAX_MSG_WITH_HEADER (HEADER_SIZE + BASIC_MSG)
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define OCTET "octet"
enum MessageOpcode
{
	ACK = 4,
	WRQ = 2,
	DATA = 3,
};

//**************************************************************************************************************
//Data_Pack - Saves a data pack.
//**************************************************************************************************************
typedef struct Data_Pack
{
	unsigned short opcode;
	unsigned short BlockNumber;
	unsigned char Data[BASIC_MSG - HEADER_SIZE];
} __attribute__((packed)) Data_Pack;

//**************************************************************************************************************
// ACK_Pack - Saves an acknowledgment pack.
//**************************************************************************************************************
typedef struct ACK_Pack
{
	unsigned short opcode;
	unsigned short BlockNumber;
} __attribute__((packed)) ACK_Pack;

//**************************************************************************************************************
// WRQ_Pack - Saves a write request pack.
//**************************************************************************************************************
typedef struct WRQ_Pack
{
	unsigned short opcode;
	char File_name[BASIC_MSG - OPCODE_SIZE];
	char Trans_mode[BASIC_MSG - OPCODE_SIZE];
} __attribute__((packed)) WRQ_Pack;

//**************************************************************************************************************
// Function name: error
// Description: Prints error and exits program.
// Parameters:  msg - error to print before system error
//**************************************************************************************************************
static void error(const char *msg, int *argv = NULL)
{
	if (msg != NULL && strcmp(msg, ""))
	{
		perror(msg);
	}
	exit(1);
}
