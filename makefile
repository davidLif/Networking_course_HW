FLAGS= -Wall -g -lm -std=gnu99 -pedantic-errors

all: client server

clean:
	-rm client client.o client_game_tools.o nim_protocol_tools.o socket_IO_tools.o Nim_game.o Game_server.o

client: client.o client_game_tools.o nim_protocol_tools.o socket_IO_tools.o
	gcc -o $@ $^ $(FLAGS)

server: Game_server.o socket_IO_tools.o nim_protocol_tools.o Nim_game.o
	gcc -o $@ $^ $(FLAGS)

client.o: client.c client.h client_game_tools.h nim_protocol_tools.h socket_IO_tools.h
	gcc -c $*.c $(FLAGS)

client_game_tools.o: client_game_tools.c client_game_tools.h nim_protocol_tools.h
	gcc -c $*.c $(FLAGS)

nim_protocol_tools.o: nim_protocol_tools.c nim_protocol_tools.h
	gcc -c $*.c $(FLAGS)

socket_IO_tools.o: socket_IO_tools.c socket_IO_tools.h
	gcc -c $*.c $(FLAGS)

Game_server.o: Game_server.c Game_server.h socket_IO_tools.h nim_protocol_tools.h Nim_game.h
	gcc -c $*.c $(FLAGS)

Nim_game.o: Nim_game.c Nim_game.h
	gcc -c $*.c $(FLAGS)
