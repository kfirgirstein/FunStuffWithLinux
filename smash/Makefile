# Makefile for the smash program
CC = g++
CXXFLAGS= -std=c++11 -Wall -Werror -pedantic-errors
CCLINK = $(CC)
OBJS = smash.o jobprocess.o JobsList.o commands.o signals.o  
RM = rm -f
TARGET= smash
# Creating the  executable

release:CXXFLAGS += -DNDEBUG
release: executable

debug: CXXFLAGS += -DDEBUG -g
debug: executable 

all: release

executable: $(OBJS)
	$(CCLINK) -o $(TARGET) $(OBJS)
# Creating the object files
jobprocess.o: jobprocess.cpp jobprocess.h
JobsList.o: JobsList.cpp JobsList.h
commands.o: commands.cpp commands.h
smash.o: smash.cpp commands.h
signals.o: signals.cpp signals.h

# Cleaning old files before new make
clean-obj:
	$(RM) *.o *~ "#"* core.*
clean:
	$(RM) $(TARGET) $(OBJS)

