#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

#include "nim_protocol_flags.h"
#include "Nim_game.h"
#include "utils.h"

#define NETWORK_FUNC_FAILURE -1
#define AWATING_CLIENTS_NUM 1

//check correctness of the input
bool checkServerArgsValidity(int argc ,const char* args[]);

//init sokect and start listening 
//On success return the listening socket number' of failure return NETWORK_FUNC_FAILURE;
int initServer(int port);

//make package for sending
bool sendResultsOfRound(int socket);

//parss recived package