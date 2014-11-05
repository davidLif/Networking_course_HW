/* define message sizes, used in the protocol*/

#define FIRST_MESSAGE_SIZE 1                       /* first message sent by the server (0 - regular, 1 - Misere */
#define SRV_RESPONSE_SIZE (1 + 4*sizeof(short))    /* response sent by the server, format:
                                                      first byte - flags (container):
																	least significant bit (1st bit): 1 iff last message sent to server was valid and accpeted
																	2nd bit:  1 iff the game ended
																	3nd bit:  if 2nd bit is 1 (game ended), contains who won (0 - client won, 1 - server won)
												      4 shorts - short i contains the number of items in stack i (first short recieved is stack 0, left most)
											       */
#define CLIENT_QUERY_SIZE    (1 + sizeof(short))	   /* query sent to the server at client's turn, format:
														  first byte: number of stack (0 is the left most, 3 is the right most)
												          1 short : amount to remove from stack
											            */
#define STACK_MESSAGE_SIZE   (4*sizeof(short)
                               

/* container - single byte that is passed in the beginning of each (except the first) message to the client (from server)*/
/* use and set flags inside the container */
void setGameOver(unsigned char* container);
void setClientWon(unsigned char* container);
void setServerWon(unsigned char* container);
void ackClientMessage(unsigned char* container);
void init_container(unsigned char* container);

/* these methods returns positive values if answer is true, 0 otherwise */
/* get flags' values from the container */
int hasServerWon(unsigned char container);
int lastMessageAcked(unsigned char container);
int hasGameEnded(unsigned char container)