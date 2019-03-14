BIN=y_ana
OBJ=
CC=g++
ENEX_LIB=$(HOME)/Documents/Uni/RANGE/Messdaten_GT2/Analyse/lib

LIB=-L$(ENEX_LIB)
INC=-I$(ENEX_LIB)

# ROOT flags
ROOTCFLAGS:=$(shell root-config --cflags)
ROOTLIBS:=$(shell root-config --libs)
ROOTGLIBS:=$(shell root-config --glibs)

## makefile settings for my setup
CFLAGS=-Wall -g $(ROOTCFLAGS) $(INC)
LDFLAGS=-Wl,-rpath,/Applications/root_v6.14.04/lib $(ROOTGLIBS)

all: $(BIN)

%.o: %.cpp
	$(CC) -c $(CFLAGS) $^

$(BIN): $(BIN).o $(OBJ)
	$(CC) $^ -o $@ $(LDFLAGS) $(CFLAGS)

clean:
	rm -f *.o
	rm -f $(BIN)
