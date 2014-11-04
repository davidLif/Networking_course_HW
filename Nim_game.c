#include "Nim_game.h";

void init_game(bool IsMisere_perrem,short heaps_size){
	int i=0;

	short heaps_init_size = heaps_init_size ;
	IsMisere = IsMisere_perrem ;
	for ( i=0 ; i<HEAPS_NUM ; i++) {//init heaps with proper size
		heaps_Array[i] = heaps_init_size;
	}
}

bool makeMove (int heapNum , short size) {
	if (heapNum >= 1 || heapNum <= HEAPS_NUM) return false;//check heapNum validity
	if (heaps_Array[heapNum-1] >= size) return false;//check that we remove no more that there is in the heap.
	else {
		heaps_Array[heapNum-1]=heaps_Array[heapNum-1]-size;//make move
		return true;
	}
}

int isVictory(int cur_player){
	int victor = 0;
	int i = 0;
	//this loop check the sizes of all the heaps- if there all empty than the game is over,else we continue;
	for ( i = 0 ; i < HEAPS_NUM ;i++) {
		if (heaps_Array[i-1] > 0) return victor;
	}
	//if we reached this point in the code, all the heaps are empty;

	//lets calculate victory for the case  IsMisere==false
	victor=cur_player;
	if (IsMisere){
		victor = victor * -1 ;//if IsMisere==true than the looser is actualy the victor. client=1,server=-1 => client=-server.
	}
	return victor;
}

void makeServerMove(){
	int maxSize_line_index = 0;
	short maxHeapSize = 0;
	int i = 0;

	//check all heaps, to find max size
	for ( i = 0 ; i < HEAPS_NUM ; i++){
		if (maxHeapSize <= heaps_Array[i-1]) {//if the heaps size is bigger than what we saw before (or equal).
			maxHeapSize = heaps_Array[i-1];
			//because we enter the if even if heaps size is equal to what we saw before and with each iteration the index grows, 
			//	if there are several heaps with the max size, maxSize_line_index will save the one with the max index.
			maxSize_line_index = i;
		}
	}
	makeMove(maxSize_line_index,1);//we remove 1 piece from the biggest heap while !isVictory, so the move is always leagel.
}

int makeRound(int player_heapNum,short player_size,bool *isLeagelMove){
	*isLeagelMove = makeMove(player_heapNum,player_size);
	if (isVictory(CLIENT)!=GAME_ON) return isVictory(CLIENT);
	makeServerMove();
	if (isVictory(SERVER)!=GAME_ON) return isVictory(SERVER);
}