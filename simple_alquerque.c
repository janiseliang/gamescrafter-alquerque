 /*************************************************************************
 **
 ** Everything below here must be in every game file
 **
 **************************************************************************/

#include <stdio.h>
#include "core/hash.h"
#include <stdlib.h>
#include <unistd.h>
#include <limits.h>
#include <ctype.h>


/*************************************************************************
**
** #defines and structs
**
**************************************************************************/
#define BLANK ' '
#define WHITE 'X' /* player 1 */
#define BLACK 'O' /* player 2 */

#define LEFT 0
#define UPLEFT 1
#define UP 2
#define UPRIGHT 3
#define RIGHT 4

/*************************************************************************
**
** Global Variables
**
*************************************************************************/

/* default values */
int gBoardWidth = 3; // must be odd
int gBoardSize = 9;
int gInitialPosition;
char* gBoard;


/**********************************************
function stuff
**********************************************/
int generic_hash_hash(char* board, int x) {
	return 1;
}


/************************************************************************
**
** NAME:        InitializeGame
**
** DESCRIPTION: Prepares the game for execution.
**              Initializes required variables.
**              Sets up gDatabase (if necessary).
**
************************************************************************/

void InitializeGame()
{	
	int i, j;
	char board[] = "OOO"
				 + "O X"
				 + "XXX";

	// for hash_init, follows "piece, min amt, max amt" and terminated by -1.
	int piece[] = {
		WHITE, 0,			gBoardSize / 2 - 1,
		BLACK, 0,			gBoardSize / 2 - 1,
		BLANK, gBoardSize,	gBoardlength - 1,
		-1
	};

	// boardsize, pieces_arr, piece restrictions func, starting player
	gNumberOfPositions = generic_hash_init(gBoardSize, piece, NULL, 0);

	gBoard = (char*)SafeMalloc(sizeof(char) * (gBoardlength + 1));
	for (i = 0; i < gBoardSize / 2; i++) {
		gBoard[i] = BLACK;
	}
	for (j = gBoardSize / 2 + 1; j < gBoardSize; j++) {
		gBoard[j] = WHITE;
	}
	gBoard[gBoardSize / 2] = BLANK;

	gBoard[gBoardSize] = '\0';
	gInitialPosition = generic_hash_hash(gBoard, 1);
}

int main(int argc, char* argv[]) {
	InitializeGame();
}