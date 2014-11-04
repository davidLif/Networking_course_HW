
#define HEAPS_NUM 4

#define CLIENT 1
#define GAME_ON 0
#define SERVER -1

//this array holds the game heaps
short heaps_Array[HEAPS_NUM];
//this variable tells us what kind of victory this is.
bool IsMisere;


//this function will fill IsMisere and heap with values
void init_game(bool IsMisere_perrem,short heaps_size);

//this function makes a move in the game according to given heap number(between 1 and HEAPS_NUM) and how much to remove(the parameter size).
//If this is an leagel move, return true. else return false.
bool makeMove (int heapNum , short size);

//this function will check all heaps and determine is the game over and who's the victor;
//return 1 for the client victory,0 if the game isn't over and -1 for server victory.
int isVictory(int cur_player);

//this function calculates what move should the server do, and execute it.
void makeServerMove();

//this function executes a whole game round- the user move (his move is given to the functions thorgth the parameters) and the computer move.
//return 1 for the client victory,0 if the game isn't over and -1 for server victory.
//fill isLeagelMove with false if the user move was ileagel,else true;
int makeRound(int player_heapNum,short player_size,bool *isLeagelMove);