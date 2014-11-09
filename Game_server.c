#include "Game_server.h"

int main( int argc, const char* argv[] ){
	int port=6423;//set the port to default
	short M;	  //initial heaps size

	//there will be two sockets- one listening at the port and one to comunicate with the server.
	int listeningSoc,toClientSocket;
	bool input_isMisere;
	struct sockaddr_in clientAdderInfo;
	bool error = false;
	int victor = 0; /* please change with definitions, what does 0 mean ? */


	socklen_t addressSize = sizeof(clientAdderInfo);//WTF why pointer??

	if (!checkServerArgsValidity(argc ,argv)){//check the validity of the argument given by the user.
		printf("unvalid arguments");
	}
	else {
		printf("got Args\n");
	}

	//set main parameters into variables
	M = atoi(argv[1]);
	printf("%d\n", M);
	if (atoi(argv[2]) == 0)	input_isMisere=false;
	else input_isMisere=true;
	if (argc==4) port=atoi(argv[3]);//save port number if given by the user
	


	listeningSoc=initServer(port);//init the server
	printf("init_server socket: %d, port: %d\n", listeningSoc, port);
	toClientSocket=accept(listeningSoc,(struct sockaddr*) &clientAdderInfo,&addressSize);
	init_game(input_isMisere,M);//init the game
	printf("init_game\n");
	printf("%d\n", heaps_Array[0]);

	//play
	error = sendGameDitails(toClientSocket,input_isMisere);
	while(victor == 0 && error ==false){
		bool isLeagelMove=false;
		char player_heapNum;
		short player_size;

		//parse message from player and save gained information.
		//printf(" wating for parse and rec\n");
		error = reciveAndParse(toClientSocket,&player_heapNum,&player_size);
		if (error) break;
		//make client and server move
		
		victor = makeRound(player_heapNum, player_size, &isLeagelMove);
		printf("Player request: heap: %d num: %d, move legal: %d\n", player_heapNum, player_size, isLeagelMove);
		printf("Current heap at serverside, 2nd heap: %d\n", heaps_Array[1]);

		//send game information to the client
		error = sendResultsOfRound(toClientSocket, isLeagelMove, victor);
		printf("Sent all results\n");
	}
	close(toClientSocket);
	close(listeningSoc);
	printf("Server exited\n");
	return 0;
}

int initServer(int port){
	struct sockaddr_in adderInfo;

	int listeningSocket=socket(AF_INET,SOCK_STREAM,0);//open IPv4 TCP socket with the default protocol.
	if (listeningSocket==-1){
		return NETWORK_FUNC_FAILURE;//Failed to create socket.
	}
	//fill sockadder struct
	adderInfo.sin_family=AF_INET;//AF_INET-
	adderInfo.sin_port=htons(port);
	adderInfo.sin_addr.s_addr=htonl( INADDR_ANY );//

	if ( bind(listeningSocket,(struct sockaddr*) &adderInfo,sizeof(adderInfo)) == -1 ){//bind the socket to the port and check success
		printf("binding error\n");
		return NETWORK_FUNC_FAILURE;
	}
	if ( listen(listeningSocket,AWATING_CLIENTS_NUM) ==-1 ){//set socket to listening to the port and check success
		printf("listening error\n");
		return NETWORK_FUNC_FAILURE;
	}
	return listeningSocket;
}

bool reciveAndParse(int socket,char *player_heapNum,short *player_size){
	char messageBuffer[3]={1};
	int isConnectionClosed,bytesSent = 3;

	int error = recv_all(socket,messageBuffer,bytesSent,&isConnectionClosed);
	if (error == 1) //if there is error
	{
		/* We should deside what should I do. */
		printf("error reciving\n");
		return true;
	}
	else {
		printf("rec OK\n");
	}
	*player_heapNum = messageBuffer[0];
	printf("%d\n", *player_heapNum);
	*player_size = ntohs(((short*)(messageBuffer+1))[0]);
	printf("%hd\n", *player_size);
	return false;
}

bool sendGameDitails(int socket,bool isMisere){
	char sendBuffer[9];
	int isConnectionClosed, bytesSent = 9;
	//short test;

	//init first byte with isMisere status.
	
	if (isMisere) sendBuffer[0]=1; 
	else sendBuffer[0] = 0;
	
	
	
	short* heap_sizes = (short*)(sendBuffer+1);
	for(int i = 0; i < HEAPS_NUM; ++i)
	{
		heap_sizes[i] = htons(heaps_Array[i]);
	}
	//send 
	int error = send_all(socket,sendBuffer,bytesSent,&isConnectionClosed);
	if (error == 1) //if there is error
	{
		/* We should deside what should I do. */
		printf("send first error ocured\n");
		return true;
	}
	return false;
}

bool sendResultsOfRound(int socket,bool last_time_validity,int victor){
	char sendBuffer[9];
	// dont use calloc
	int isConnectionClosed, bytesSent=9;

	unsigned char flag_container = sendBuffer[0];
	short* heap_sizes = (short*)(sendBuffer+1);
	// init container byte, contains game flags and information
	init_container(&flag_container);

	//This code chunk fills the first byte with all the nessecery information 
	switch (victor){
		//fill game over and  who's victory
		case CLIENT:
			
		
			setClientWon(&flag_container);
			break;
		case SERVER:
			
			
			setServerWon(&flag_container);
			break;
		default:
			// don't turn on game over flag
			break;
		
	}
	// shouldn't it be, last_time_validity == true iff last time was valid?
	if (!last_time_validity)//if the previous move was correct' set proper bit in message.
	{
		ackClientMessage(&flag_container);
	}
	
	//fill the rest of the message (heap sizes)
	for (int i = 0 ;i < HEAPS_NUM ; i++){
		heap_sizes[i] = htons(heaps_Array[i]);
	}

	//sent
	
	int error = send_all(socket,sendBuffer,bytesSent,&isConnectionClosed);
	printf("SENT STACKS\n");
	if (error == 1) //if there is error
	{
		printf("sending error\n");
		return true;
	}
	return false;
}

bool checkServerArgsValidity(int argc,const char* argv[]){
	if (1 > atoi(argv[1]) || atoi(argv[1]) > 1500) return false;//check if M is out range
	if (argv[2][0] != '0' && argv[2][0] != '1') return false;//check isMisere validity
	return true;
}
