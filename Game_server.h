#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "socket_IO_tools.h"
#include "nim_protocol_tools.h"
#include "Nim_game.h"

#define NETWORK_FUNC_FAILURE -1
#define AWATING_CLIENTS_NUM 1

//check correctness of the input
bool checkServerArgsValidity(int argc ,const char* args[]);

//init sokect dedicated for listening to the given port and start listening 
//On success return the listening socket number.In case of failure return NETWORK_FUNC_FAILURE;
int initServer(int port);

//make regular package for sending the information about this game round to the client.
//socket is the socket with which we send the data to the client
//bool last_time_validity should be ture if the last move done by user was leagel. else false
//int victor should be the reasult of makeRound(of the same format at least).
//return false if successeded, else true
bool sendResultsOfRound(int socket,bool last_time_validity,int victor);

//make the first-time package for sending the information about the game to the client
//isMisere should be according to the game type
//socket is the socket with which we send the data to the client
//return false if successeded, else true
bool sendGameDitails(int socket,bool isMisere);

//parse recived package form the client and fill player_heapNum and player_size with the information about the client move
//socket is the socket with which we send the data to the client
//return false if successeded, else true
bool reciveAndParse(int socket,char *player_heapNum,short *player_size);

//This function checks if the arguments given to the server are leagel according to the program definition
bool checkServerArgsValidity(int argc,const char* argv[]);