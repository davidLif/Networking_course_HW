#include "Game_server.h"

int main( int argc, const char* argv[] ){
	int port=6423;//set the port to default
	short M;//initial heaps size
	bool input_isMisere;//initial the kind of game(if the last one to finish the last heap is the winner or the looser)

	//there will be two sockets- one listening at the port and one to comunicate with the server.
	int listeningSoc,toClientSocket;
	//in this variable we save the client's address information
	struct sockaddr_in clientAdderInfo;
	socklen_t addressSize = sizeof(clientAdderInfo);
	//this variable resives true if an error occured during the communication with the client
	bool error = false;
	// this variable tells us us if the game has ended, and who is the winner in such a case
	int victor = 0; 

	
	//check the validity of the argument given by the user.
	if (!checkServerArgsValidity(argc ,argv)){
		printf("unvalid arguments");
	}

	//set main parameters into variables
	M = atoi(argv[1]);
	if (atoi(argv[2]) == 0)	input_isMisere=false;
	else input_isMisere=true;
	if (argc==4) port=atoi(argv[3]);//save port number if given by the user

	//init the server
	listeningSoc=initServer(port);
	printf("init_server socket: %d, port: %d\n", listeningSoc, port);
	if (listeningSoc==-1) //if failed to create listening socket to the wanted port
	{
		printf("failed to create listening socket\n");
		return 0;
	}
	//accept the connection from the client
	toClientSocket=accept(listeningSoc,(struct sockaddr*) &clientAdderInfo,&addressSize);
	//init the game according to parameters 
	init_game(input_isMisere,M);
	printf("init_game\n");

	//play the game with the user

	//send the first packadge to the client (which is slietly differnt from all the others)
	error = sendGameDitails(toClientSocket,input_isMisere);
	//play the game while it is still on (victor == 0) and no error acorred (error ==false)
	while(victor == 0 && error ==false){
		bool isLeagelMove=false;//this variable tells us if the move done this round is leagel or not
		char player_heapNum;//this variable holds the number of heap the player chose this round for his move
		short player_size;//this variable holds the number of objects the player takes from the heap this round

		//parse message from player and save gained information.
		error = reciveAndParse(toClientSocket,&player_heapNum,&player_size);
		if (error) break;

		//make client and server move
		victor = makeRound(player_heapNum,player_size,&isLeagelMove);
		printf("Player request: heap: %d num: %d, move legal: %d\n", player_heapNum, player_size, isLeagelMove);

		//send game information to the client
		error = sendResultsOfRound(toClientSocket,isLeagelMove,victor);
		printf("Sent the round results to the client\n");
	}
	if (victor != 0)
	{
		printf("The game is over\n");
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
	//fill sockadder structwith correct parameters
	adderInfo.sin_family=AF_INET;
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
	char messageBuffer[3];
	//bytesSent tells the send func how many bytes to send.
	//isConnectionClosed- this variable tells us if the connection was ended fro the clients side or is it enother kind of data-sending error.
	int isConnectionClosed,bytesSent = 3;

	//fill the messageBuffer with the packadge 
	int error = recv_all(socket,messageBuffer,bytesSent,&isConnectionClosed);
	if (error == 1) //if there is error
	{
		printf("error reciving move from the client\n");
		return true;
	}
	//fill the client move parameters according to the information stoared in messageBuffer
	*player_heapNum = messageBuffer[0];
	*player_size = ntohs(((short*)(messageBuffer+1))[0]);
	return false;
}

bool sendGameDitails(int socket,bool isMisere){
	char sendBuffer[SERVER_MESSAGE_SIZE];
	//bytesSent tells the send func how many bytes to send.
	//isConnectionClosed- this variable tells us if the connection was ended fro the clients side or is it enother kind of data-sending error.
	int isConnectionClosed, bytesSent = SERVER_MESSAGE_SIZE;

	//init first byte with isMisere status.
	if (isMisere) sendBuffer[0]=1; 
	else sendBuffer[0] = 0;
	//this is a pointer to the place in the message buffer where we will put the heaps sizes
	short* heap_sizes = (short*)(sendBuffer+1);
	//fill the rest of the message (heap sizes)
	for(int i = 0; i < HEAPS_NUM; ++i)
	{
		heap_sizes[i] = htons(heaps_Array[i]);
	}
	//Send the first message to the client
	int error = send_all(socket,sendBuffer,bytesSent,&isConnectionClosed);
	if (error == 1) //if there is error
	{
		if (isConnectionClosed == 1)
		{
			printf("The client closed the connection on his side\n");
		}
		else{
			printf("There are problems with the communications\n");
		}
		return true;
	}
	return false;
}

bool sendResultsOfRound(int socket,bool last_time_validity,int victor){
	char sendBuffer[SERVER_MESSAGE_SIZE];
	//bytesSent tells the send func how many bytes to send.
	//isConnectionClosed- this variable tells us if the connection was ended fro the clients side or is it enother kind of data-sending error.
	int isConnectionClosed, bytesSent=SERVER_MESSAGE_SIZE;
	//on this varable we set all the "first byte" information (later we will add it into the buffer's first byte)
	unsigned char flag_container = 0;
	//this is a pointer to the place in the message buffer where we will put the heaps sizes
	short* heap_sizes = (short*)(sendBuffer+1);
	// init container byte, contains game flags and information
	init_container(&flag_container);

	//This code chunk fills the first byte with all the nessecery information 
	switch (victor){
		//fill game over and  who's victory
		case CLIENT:
			printf("The client win\n");
			setClientWon(&flag_container);
			break;
		case SERVER:
			printf("The server win\n");
			setServerWon(&flag_container);
			break;
		default:
			// don't turn on game over flag
			break;
		
	}
	//if the previous move was ileagel, set proper bit in message.
	if (last_time_validity)
	{
		ackClientMessage(&flag_container);
	}
	//fill the first message buffer byte accordingly to the flag_container,which carries the information needed for the first byte this round
	sendBuffer[0]=(char)flag_container;

	//fill the rest of the message (heap sizes)
	for (int i = 0 ;i < HEAPS_NUM ; i++){
		heap_sizes[i] = htons(heaps_Array[i]);
	}
	
	//Send the message to the client
	int error = send_all(socket,sendBuffer,bytesSent,&isConnectionClosed);
	if (error == 1) //if there is error
	{
		if (isConnectionClosed == 1)
		{
			printf("The client closed the connection on his side\n");
		}
		else{
			printf("There are problems with the communications\n");
		}
		return true;
	}
	return false;
}

bool checkServerArgsValidity(int argc,const char* argv[]){
	if (1 > atoi(argv[1]) || atoi(argv[1]) > 1500) return false;//check if M is out range
	if (argv[2][0] != '0' && argv[2][0] != '1') return false;//check isMisere validity
	return true;
}
