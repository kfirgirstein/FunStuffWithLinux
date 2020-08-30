# Makefile for the bank program
CC = g++
#CXXFLAGS = -std=c++11 -Wall
CXXFLAGS = -std=c++11 -Wall -Werror -pedantic-errors 
CCLINK = $(CC)
OBJS = main.o tftpServer.o MessageHandler.o
RM = rm -f
TARGET = ttftp
# Creating the  executable
release: CXXFLAGS += -DNDEBUG
release: executable 

debug: CXXFLAGS += -DDEBUG -g
debug: executable 

all: release

executable: $(OBJS)
	$(CCLINK) $(CFLAGS) -o $(TARGET) $(OBJS)
# Creating the object files
MessageHandler.o: MessageHandler.cpp MessageHandler.h Messages.h
tftpServer.o: tftpServer.cpp tftpServer.h MessageHandler.h
main.o: main.cpp tftpServer.h MessageHandler.h
# Cleaning old files before new make
clean-obj:
	$(RM) *.o *~ "#"* core.*
clean:
	$(RM) $(TARGET) $(OBJS)

