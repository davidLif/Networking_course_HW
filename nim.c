

#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <limits.h>
#include <errno.h>
#include "nim_protocol_tools.h"

#define DEFAULT_HOST    "localhost"
#define DEFAULT_PORT    "6325"

#define INPUT_ERR       "Error: invalid arguments\n"
#define ADDR_ERR        "Error: could not retrieve server IPv4 address\n"

int send_all(int sockfd, const void * buffer, int* num_bytes, int* connection_closed);

/* frees given resources, input server_info == null iff that resources
   was already freed before 
*/
void free_resources(int sockfd, addrinfo* server_info)
{
	if(close(sockfd))
	{
		printf("Error while closing socket: %s", strerr(errno));
	}
	if(server_info != NULL)
		freeaddrinfo(server_info);
}

static int sockfd; /* socket to connect to the server */
int main(int argc, char* argv[])
{

	char* host_name = DEFAULT_HOST;
	char* server_port = DEFAULT_PORT;
	if(argc > 1)
	{
		
		// host given
		host_name = argv[1];
		
		if(argc == 3)
		{
			// both host and port were given
			server_port = argv[2];
		}
		else if (argc > 3)
		{
			printf(INPUT_ERR);
			exit(1);
		}
	}
	else
	{
		// no arguments were given
		printf(INPUT_ERR);
		exit(1);
	}

	/* lets connect to the nim server */
	struct addrinfo hints;
	struct addrinfo* server_info; /* will hold getaddrinfo result */
	int err_code;
	
	memset(&hints, 0, sizeof(hints));

	/* we require an IPv4 TCP socket */
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;

	/* fetch address of given host_name with given server_port */
	if(err_code = getaddrinfo(host_name, server_port, &hints, &server_info)
	{
		printf("%s: %s\n", ADDR_ERR, gai_strerror(err_code));
		exit(1);
	}

	/* we may connect to the first result */
	/* create socket */
	sockfd = socket(server_info->ai_family, server_info->ai_socktype, server_info->ai_protocol);
	if(sockfd  == -1)
	{
		printf("Failed to create socket: %s", strerr(errno));
		freeaddrinfo(server_info);
		exit(1);
	}

	/* connect to server */
	if(connect(sockfd, server_info->ai_addr, server_info->ai_addrlen ) == -1)
	{
		/* failed to connect to server, free resources */
		printf("Failed to connect: %s", strerr(errno));
		free_resources(sockfd, server_info);
		exit(1);

	}
	
	/* free used resources */
	freeaddrinfo(server_info);

}

/* this method prints the type of game line. if game is regular then isMisere == 0, otherwise positive */

#define MISERE_GAME_MSG "This is a Misere game"
#define REGULAR_GAME_MSG "This is a Regular game"
void print_game_type(unsigned char isMisere)
{
	
	printf("%s\n", isMisere ? MISERE_GAME_MSG : REGULAR_GAME_MSG);
	
}
#define GAME_TITLE "nim"
void print_title(void)
{
	printf("%s\n", GAME_TITLE);
}

void show_stack_sizes()
{
	// first receive the data from the server
	int closed_connection;
	unsigned char buffer[STACK_MESSAGE_SIZE];
	if(recv_all(sockfd, buffer, STACK_MESSAGE_SIZE, &closed_connection))
	{
		// error, 
		if(closed_connection)
		{
			/* other end was closed (unexpectedly) */
			print_closed_connection();
		}
		else
		{
			/* different error */
			printf("Error: could not recieve data %s\n", strerr(errno));
		}
		
		free_resources(sockfd, NULL);
		exit(0);
	}
	// otherwise, we have successfully recieved stacks' sizes, print them
	print_stacks((unsigned short*)buffer);

}


/* protocol implementation part 
   this method communicates with the server to play nim
   sockfd should hold a valid socket descriptor to communicate with the server
*/


void play_nim()
{
	int closed_connection = 0;
	unsigned char game_type;

	/* print nim title */
	print_title();

	/* read the openning message, isMisere*/
	if(recv_all(sockfd, &game_type, FIRST_MESSAGE_SIZE, &closed_connection))
	{
		// error
		if(closed_connection)
		{
			/* other end was closed */
			print_closed_connection();
		}
		else /* different error */
			printf("Failed to receive data: %s\n", strerr(errno));
		free_resources(sockfd, NULL);
		return;
	}
	
	print_game_type(game_type);

	// read stack sizes from server


	unsigned char game_status;	  /* first byte of each message recieved from the server (except the first unique message that only describes game type)
									 contains game status flags as described in the protocol
								  */
	int first = 1;			      /* simple flag to indiciate whether it is the first time we're printing the stacks */
	
	/* reset game_status (game has not ended), cannot end when no one has made a move */
	init_container(&game_status);

	while(!hasGameEnded(game_status))
	{
		if(first)
		{
			first = 0;
		}
		else
		{
			// also print if last message was accepted or not
			print_message_acked(game_status);
		}

		// get stacks' sizes from the server, and print them to the user
		show_stack_sizes();
		
		// read user input and act accordingly (send query if input is not Q)
		handle_user_move();

		// recieve status byte from server
		if(recv_all(sockfd, &game_status, sizeof(char), &closed_connection))
		{
			// error
			if(closed_connection)
			{
				/* other end was closed */
				print_closed_connection();
			}
			else /* different error */
				printf("Failed to receive data: %s\n", strerr(errno));
			free_resources(sockfd, NULL);
			return;
		}

	}
	/* if we have reached this part, game has ended, print the winner's text */
	print_winner(game_status);
	return;
	

	
}

#define SERVER_WON_STR "I win!\n"
#define CLIENT_WON_STR "You win!\n"
void print_winner(unsigned char game_status)
{
	if(hasServerWon(game_status))
	{
		printf(SERVER_WON_STR);
	}
	else
		printf(CLIENT_WON_STR);
}

#define MESSAGE_ACKED_STR "Move accepted\n"
#define MESSAGE_DECLINED_STR "Illegal move\n"
void print_message_acked(unsigned char game_status)
{
	if(lastMessageAcked(game_status))
	{
		printf(MESSAGE_ACKED_STR);
	}
	else
		printf(MESSAGE_DECLINED_STR);
}


/* print the number of items in each stack
 
*/

void print_stacks(unsigned short* stacks)
{
	printf("Heap sizes are %d, %d, %d, %d\n", ntohs(stacks[0]), ntohs(stacks[1]), ntohs(stacks[2]), ntohs(stacks[3]));
	
}



void handle_user_move()
{
	char req; /* A, B, C, D or Q */
	printf("Your turn:\n");
	if(scanf("%c", &req) != 1)
	{
		printf("Error: could not read input from user\n");
		free_resources(sockfd, NULL);
		exit(0);
	}
	if(req == 'Q')
	{
		free_resources(sockfd, NULL);
		exit(0);
	}
	unsigned char stack_num = 0;
	if(req == 'A' || req == 'B' || req == 'C' || req == 'D')
	{
		stack_num = req - 'A';
	}
	else
	{
		// error, invalid input
		printf("Error: invalid user input\n");
		free_resources(sockfd, NULL);
		exit(0);
	}
	
	// read number of items to remove from user
	unsigned short items_to_remove;
	if(scanf("%hu", &items_to_remove) != 1)
	{
		printf("Error: could not read input from user\n");
		free_resources(sockfd, NULL);
		exit(0);
	}

	// build request from server: byte 1 : stack number (byte), next one short: items_to_remove
	unsigned char client_query[CLIENT_QUERY_SIZE];
	client_query[0] = stack_num;
	unsigned short* p = (unsigned short*)(client_query + 1);
	p[0] = htons(items_to_remove);

	int connection_closed;

	// send the query
	if(send_all(sockfd, client_query, CLIENT_QUERY_SIZE, &connection_closed))
	{
		// error
		if(closed_connection)
		{
			/* other end was closed. */
			print_closed_connection();
		}
		else
			printf("Failed to send data: %s\n", strerr(errno));
		free_resources(sockfd, NULL);
		exit(0);
		
	}
	return;

}
