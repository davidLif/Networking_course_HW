#include "Game_server.h"

int main( int argc, const char* argv[] ){
	int port=6423;//set the port to default
	int M;//initial heaps size
	//there will be two sockets- one listening at the port and one to comunicate with the server.
	int listeningSoc,toClientSocket;
	bool input_isMisere;
	struct sockaddr_in clientAdderInfo;
	bool error = false;
	int victor = 0;
	socklen_t addressSize = sizeof(clientAdderInfo);//WTF why pointer??

	if (!checkServerArgsValidity(argc ,argv)){//check the validity of the argument given by the user.
		printf("unvalid arguments");
	}

	//set main parameters into variables
	M = atoi(argv[0]);
	if (atoi(argv[0]) == 0)	input_isMisere=false;
	else input_isMisere=true;
	if (argc==3) port=atoi(argv[2]);//save port number' if given by the user
	
	listeningSoc=initServer(port);//init the server
	toClientSocket=accept(listeningSoc,(struct sockaddr*) &clientAdderInfo,&addressSize);
	init_game(input_isMisere,M);//init the game

	//play
	error = sendGameDitails(toClientSocket,input_isMisere);
	while(victor == 0 && error ==true){
		bool isLeagelMove,error;
		char player_heapNum;
		short player_size;

		//parse message from player and save gained information.
		error = reciveAndParse(toClientSocket,&player_heapNum,&player_size);
		if (error) break;
		//make client and server move
		victor = makeRound(player_heapNum,player_size,&isLeagelMove);
		//send game information to the client
		error = sendResultsOfRound(toClientSocket,isLeagelMove,victor);
	}
	close(toClientSocket);
	close(listeningSoc);
	printf("Server exited\n");
	return 0;
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
	adderInfo.sin_addr.s_addr=htonl( INADDR_ANY );//

	if ( bind(listeningSocket,(struct sockaddr*) &adderInfo,sizeof(adderInfo)) == -1 ){//bind the socket to the port and check success
		return NETWORK_FUNC_FAILURE;
	}
	if ( listen(listeningSocket,AWATING_CLIENTS_NUM) ==-1 ){//set socket to listening to the port and check success
		return NETWORK_FUNC_FAILURE;
	}
	return listeningSocket;
}

bool reciveAndParse(int socket,char *player_heapNum,short *player_size){
	char messageBuffer[3];
	int *isConnectionClosed = NULL,bytesSent = 3;

	int error = recv_all(socket,messageBuffer,bytesSent,isConnectionClosed);
	if (error == 1) //if there is error
	{
		/* We should deside what should I do. */
		return true;
	}
	*player_heapNum = messageBuffer[0];
	*player_size = bytesArrayIntoShort(messageBuffer + 1);
	return false;
}

bool sendGameDitails(int socket,bool isMisere){
	char *sendBuffer=calloc(9,sizeof(char));
	int *isConnectionClosed = NULL, bytesSent = 9;

	//init first byte with isMisere status.
	init_container((unsigned char *)sendBuffer);
	if (isMisere) sendBuffer[0]=1; //not need for else since init_container filled the first char with 0.

	//fill the rest of the message (only if nedded)
	for (int i=0;i<HEAPS_NUM;i++){
		shortIntoBytesArray(heaps_Array[i],sendBuffer+i*2+1);//fiil the heap sizes
	}

	//send 
	int error = send_all(socket,sendBuffer,bytesSent,isConnectionClosed);
	if (error == 1) //if there is error
	{
		/* We should deside what should I do. */
		return true;
	}
	return false;
}

bool sendResultsOfRound(int socket,bool last_time_validity,int victor){
	char *sendBuffer=NULL;
	int *isConnectionClosed = NULL, bytesSent=1;

	//This code chunk fill the first byte with all the nessecery information and will allocate the buffer.
	switch (victor){//fill game over and  who's victory
		case CLIENT:
			sendBuffer=calloc(1,sizeof(char));
			init_container((unsigned char *)sendBuffer);
			setClientWon((unsigned char *)sendBuffer);
			break;
		case SERVER:
			sendBuffer=calloc(1,sizeof(char));
			init_container((unsigned char *)sendBuffer);
			setServerWon((unsigned char *)sendBuffer);
			break;
		default://must be GAME_ON
			//we will have send the "status byte"(the first byte) and the heaps sizes.
			sendBuffer=calloc(9,sizeof(char));
			bytesSent = 9;
			//we will fill the first byte with zero's. 
			//if the game is on than the only 1 possibale is the validity of the previous move,
			//	 which we will fill later.
			init_container((unsigned char *)sendBuffer);
	}
	if (last_time_validity)//if the previous move was correct' set proper bit in message.
	{
		ackClientMessage((unsigned char *)sendBuffer);
	}

	//fill the rest of the message (only if nedded)
	for (int i = 0 ;i < HEAPS_NUM && victor == GAME_ON ; i++){
		shortIntoBytesArray(heaps_Array[i],sendBuffer+i*2+1);//fiil the heap sizes
	}

	//sent
	int error = send_all(socket,sendBuffer,bytesSent,isConnectionClosed);
	if (error == 1) //if there is error
	{
		/* We should deside what should I do. */
		return true;
	}
	return false;
}