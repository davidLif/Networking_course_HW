FLAGS= -Wall -g -lm -std=c99 -pedantic-errors

all: client

clean:
	-rm client client.o client_game_tools.o nim_protocol_tools.o socket_IO_tools.o

client: client.o client_game_tools.o nim_protocol_tools.o socket_IO_tools.o
	gcc -o $@ $^ $(FLAGS)

client.o: client.c client.h client_game_tools.h nim_protocol_tools.h socket_IO_tools.h
	gcc -c $*.c $(FLAGS)

client_game_tools.o: client_game_tools.c client_game_tools.h
	gcc -c $*.c $(FLAGS)

nim_protocol_tools.o: nim_protocol_tools.c nim_protocol_tools.h
	gcc -c $*.c $(FLAGS)

socket_IO_tools.o: socket_IO_tools.c socket_IO_tools.h
	gcc -c $*.c $(FLAGS)
