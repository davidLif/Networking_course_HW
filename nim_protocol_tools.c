#include "nim_protocol_tools.h"

/* container - single byte that is passed in the beginning of each (regular) message to the client */

/* number of bit shifts required to get the flags */
#define ACK_NUM_SHIFTS 0
#define GAME_OVER_NUM_SHIFTS 1
#define SERVER_WON_NUM_SHIFTS 2

/* return information from the given container (byte) */
int hasGameEnded(unsigned char container)
{
	container >>= GAME_OVER_NUM_SHIFTS;
	return container & 1;
}

int lastMessageAcked(unsigned char container)
{
	container >>= ACK_NUM_SHIFTS;
	return container && 1;
}

int hasServerWon(unsigned char container)
{
	container >>= SERVER_WON_NUM_SHIFTS;
	return container && 1;
}


/* reset container */
void init_container(unsigned char* container)
{
	/* no ack, game continues */
	*container = 0;
}

/* accept client's last message */
void ackClientMessage(unsigned char* container)
{
	unsigned char flag = 1 << ACK_NUM_SHIFTS;
	(*container) |= flag;
}
void setServerWon(unsigned char* container)
{
	unsigned char flag = 1 << SERVER_WON_NUM_SHIFTS;
	(*container) |= flag;
	setGameOver(container);
}
void setClientWon(unsigned char* container)
{
	/* by default, 3rd bit is 0 indiciating that client won
	   all we need to do is set the game over bit to 1 */
	setGameOver(container);
}

void setGameOver(unsigned char* container)
{
	unsigned char flag = 1 << GAME_OVER_NUM_SHIFTS;
	(*container) |= flag;

}