ifndef CC
CC = $(CC)
endif
#CFLAGS=-Wall -O2
CFLAGS=-Wall -O2 -DHEX
INCLUDES= -I.
LFLAGS= -L.
CC = gcc
EXE = .exe

all: compile_mu0$(EXE) execute_mu0$(EXE)

compile_mu0$(EXE): compile_mu0.o
	$(CC) compile_mu0.o $(LFLAGS) -o compile_mu0$(EXE)

execute_mu0$(EXE): execute_mu0.o
	$(CC) execute_mu0.o $(LFLAGS) -o execute_mu0$(EXE)

tidy:
	rm -f *.o core *~

clean:
	rm -f compile_mu0$(EXE) execute_mu0$(EXE) *.o core *~
