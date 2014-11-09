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
#include "utils.h"

#define NETWORK_FUNC_FAILURE -1
#define AWATING_CLIENTS_NUM 1

//check correctness of the input
bool checkServerArgsValidity(int argc ,const char* args[]);

//init sokect and start listening 
//On success return the listening socket number' of failure return NETWORK_FUNC_FAILURE;
int initServer(int port);

//make regular package for sending. int victory should be the reasult of makeRound(same format at least).
//return false if successeded, else true
bool sendResultsOfRound(int socket,bool last_time_validity,int victor);

//special package
//return false if successeded, else true
bool sendGameDitails(int socket,bool isMisere);

//parss recived package and fill player_heapNum and player_size
//return false if successeded, else true
bool reciveAndParse(int socket,char *player_heapNum,short *player_size);

bool checkServerArgsValidity(int argc,const char* argv[]);