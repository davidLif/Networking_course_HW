#include "Game_server.h"

void main( int argc, const char* argv[] ){
	int port=6423;//set the port to default

	if (!checkServerArgsValidity(argc ,argv)){//check the validity of the argument given by the user.
		printf("unvalid arguments");
	}
	if (argc==3) port=atoi(argv[2]);//save port number' if given by the user
	

}

int initServer(int port){
	struct sockaddr_in adderInfo;

	int listeningSocket=socket(PF_INET,SOCK_STREAM,0);//open IPv4 TCP socket with the default protocol.
	if (listeningSocket==-1){
		return NETWORK_FUNC_FAILURE;//Failed to create socket.
	}
	//fill sockadder struct
	adderInfo.sin_family=AF_INET;//AF_INET-
	adderInfo.sin_port=htons(port);
	adderInfo.sin_addr=htnol( INADDR_ANY );

	if ( bind(listeningSocket,adderInfo,sizeof(adderInfo)) == -1 ){//bind the socket to the port and check success
		return NETWORK_FUNC_FAILURE;
	}
	if ( listen(listeningSocket,AWATING_CLIENTS_NUM) ==-1 ){//set socket to listening to the port and check success
		return NETWORK_FUNC_FAILURE;
	}
	return listeningSocket;
}

bool sendResultsOfRound(int socket){
	char sendBuffer[9];
	for (int i=0;i<HEAPS_NUM;i++){
		shortIntoBytesArray(heaps_Array[i],sendBuffer+i*2+1);//fiil the heap sizes
	}

}