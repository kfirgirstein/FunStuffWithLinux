# Makefile for the bank program
CC = g++
CXXFLAGS = -std=c++11 -Wall -pthread
#CXXFLAGS = -std=c++11 -Wall -Werror -pedantic-errors -pthread
CCLINK = $(CC) -pthread
OBJS = main.o ATM.o Bank.o Account.o AccountController.o Logger.o
RM = rm -f
TARGET = bank
# Creating the  executable
release: CXXFLAGS += -DNDEBUG
release: executable 

debug: CXXFLAGS += -DDEBUG -g
debug: executable 

all: release

executable: $(OBJS)
	$(CCLINK) $(CFLAGS) -o $(TARGET) $(OBJS)
# Creating the object files
Account.o: Account.cpp Account.h
AccountController.o: AccountController.cpp AccountController.h Account.h
Bank.o: Bank.cpp Bank.h
ATM.o: ATM.cpp ATM.h
Logger.o: Logger.cpp Logger.h
main.o: main.cpp ATM.h Bank.h Logger.h
# Cleaning old files before new make
clean-obj:
	$(RM) *.o *~ "#"* core.*
clean:
	$(RM) $(TARGET) $(OBJS)

