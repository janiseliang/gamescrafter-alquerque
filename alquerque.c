// $id$
// $log$

/*
 * The above lines will include the name and log of the last person
 * to commit this file to CVS
 */

 /************************************************************************
 **
 ** NAME:        malquerque.c
 **
 ** DESCRIPTION: Alquerque
 **
 ** AUTHOR:      Janise Liang
 **
 ** DATE:        2020-10-15
 **
 ** UPDATE HIST:
 **
 **************************************************************************/

 /*************************************************************************
 **
 ** Everything below here must be in every game file
 **
 **************************************************************************/

#include <stdio.h>
#include "gamesman.h"
#include <stdlib.h>
#include <unistd.h>
#include <limits.h>
#include <ctype.h>

 /*************************************************************************
 **
 ** Game-specific constants
 **
 **************************************************************************/

STRING kGameName = "Alquerque";   /* The name of your game */
STRING kAuthorName = "Janise Liang";   /* Your name(s) */
STRING kDBName = "alquerque";   /* The name to store the database under */

BOOLEAN kPartizan = TRUE;   /* A partizan game is a game where each player has different moves from the same board (chess - different pieces) */
BOOLEAN kGameSpecificMenu = TRUE;   /* TRUE if there is a game specific menu. FALSE if there is not one. */
BOOLEAN kTieIsPossible = FALSE;   /* TRUE if a tie is possible. FALSE if it is impossible.*/
BOOLEAN kLoopy = TRUE;   /* TRUE if the game tree will have cycles (a rearranger style game). FALSE if it does not.*/

BOOLEAN kDebugMenu = TRUE;   /* TRUE only when debugging. FALSE when on release. */
BOOLEAN kDebugDetermineValue = FALSE;   /* TRUE only when debugging. FALSE when on release. */

POSITION gNumberOfPositions = 0; /* The number of total possible positions | If you are using our hash, this is given by the hash_init() function*/
POSITION gInitialPosition = 0; /* The initial hashed position for your starting board */
POSITION kBadPosition = -1; /* A position that will never be used */
void* gGameSpecificTclInit = NULL;

/*
 * Help strings that are pretty self-explanatory
 * Strings than span more than one line should have backslashes (\) at the end of the line.
 */

STRING kHelpGraphicInterface =
"Not written yet";

STRING kHelpTextInterface =
"On your turn, select a piece to move. If a capture is possible, \n\
you must make a capture.";

STRING kHelpOnYourTurn =
"On your turn, type the piece that you want to move and then the \n\
direction you'd like to move. Directions are: \n\n\
\t\tur u  ul\n\
\t\t l  X  r \n\
\t\tdl  d  dr\n\
";

STRING kHelpStandardObjective =
"Capture all of your opponent's pieces to win. You can also win \n\
if your opponent does not have any more moves possible.";

STRING kHelpReverseObjective =
"N/A";

STRING kHelpTieOccursWhen =
"N/A";

STRING kHelpExample =
"Not implemented";


/*************************************************************************
**
** #defines and structs
**
**************************************************************************/
#define BLANK ' '
#define WHITE 'W' /* player 1 */
#define BLACK 'B' /* player 2 */

/*
#define BLANK 0
#define WHITE 1
#define BLACK 2
*/

#define LEFT 0
#define UPLEFT 1
#define UP 2
#define UPRIGHT 3
#define RIGHT 4
#define DOWNRIGHT 5
#define DOWN 6
#define DOWNLEFT 7

/*************************************************************************
**
** Global Variables
**
*************************************************************************/

/* default values */
int gBoardWidth = 3; // must be odd
int gBoardSize;
char* gBoard;

// settings
BOOLEAN fixedAtBoardEnd = TRUE;
// BOOLEAN noRepeats = FALSE; // not implemented

/*************************************************************************
**
** Function Prototypes
**
*************************************************************************/

/* External */
#ifndef MEMWATCH
extern GENERIC_PTR      SafeMalloc();
extern void             SafeFree();
#endif
extern POSITION         generic_hash_init(int boardsize, int pieces_array[], int (*vcfg_function_ptr)(int* cfg), int player);
extern POSITION         generic_hash_hash(char* board, int player);
extern char* generic_hash_unhash(POSITION hash_number, char* empty_board);
extern int              generic_hash_turn(POSITION hashed);

/* Internal */
void                    InitializeGame();
MOVELIST* GenerateMoves(POSITION position);
POSITION                DoMove(POSITION position, MOVE move);
VALUE                   Primitive(POSITION position);
void                    PrintPosition(POSITION position, STRING playersName, BOOLEAN usersTurn);
//void                    printBoard(char board[]);
void                    PrintComputersMove(MOVE computersMove, STRING computersName);
void                    PrintMove(MOVE move);
STRING                  MoveToString(MOVE);
USERINPUT               GetAndPrintPlayersMove(POSITION position, MOVE* move, STRING playersName);
BOOLEAN                 ValidTextInput(STRING input);
MOVE                    ConvertTextInputToMove(STRING input);
void                    GameSpecificMenu();
void                    SetTclCGameSpecificOptions(int options[]);
POSITION                GetInitialPosition();
BOOLEAN                 GetInitPosHelper(char* board, char playerchar);
int                     NumberOfOptions();
int                     getOption();
void                    setOption(int option);
void                    DebugMenu();

// game-specific
int						canMoveDir(int arraynum, int dir, int current);
int						getColumn(int i);
int						getRow(int i);
MOVE					EncodeMove(int dir, int col, int row, int isJump);
int						GetDirection(MOVE mv);
int						GetXCoord(MOVE mv);
int						GetYCoord(MOVE mv);
int						GetMoveIsJump(MOVE mv);
int						getArraynum(int x, int y);
int						isAtBoardEnd(int arraynum, int player);

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
	gBoardSize = gBoardWidth * gBoardWidth;

	// for hash_init, follows "piece, min amt, max amt" and terminated by -1.
	int piece[] = {
		WHITE, 0,	gBoardSize / 2,
		BLACK, 0,	gBoardSize / 2,
		BLANK, 1,	gBoardSize - 2,
		-1
	};

	// boardsize, pieces_arr, piece restrictions func, starting player
	gNumberOfPositions = generic_hash_init(gBoardSize, piece, NULL, 0);
	printf("\ngNumberOfPositions: %lld\n", gNumberOfPositions);

	//gNumberOfPositions = pow(3, gBoardSize);

	//gBoard = (int*)SafeMalloc(sizeof(int) * (gBoardSize + 1));
	gBoard = (char*)SafeMalloc(sizeof(char) * (gBoardSize + 1));
	for (i = 0; i < gBoardSize / 2; i++) {
		gBoard[i] = BLACK;
	}
	for (j = gBoardSize / 2 + 1; j < gBoardSize; j++) {
		gBoard[j] = WHITE;
	}
	gBoard[gBoardSize / 2] = BLANK;

	gBoard[gBoardSize] = '\0';

	gInitialPosition = generic_hash_hash(gBoard, 1);
	//gInitialPosition = hash(gBoard, 0);
	printf("This is the initialPosition: %lld\n", gInitialPosition);

	// Testing custom starters
	gInitialPosition = 13993;
	//generic_hash_unhash(gInitialPosition, gBoard);

	gMoveToStringFunPtr = &MoveToString;
}


/************************************************************************
**
** NAME:        GenerateMoves
**
** DESCRIPTION: Creates a linked list of every move that can be reached
**              from this position. Returns a pointer to the head of the
**              linked list.
**
** INPUTS:      POSITION position : Current position for move
**                                  generation.
**
** OUTPUTS:     (MOVELIST *)      : A pointer to the first item of
**                                  the linked list of generated moves
**
** CALLS:       MOVELIST *CreateMovelistNode();
**
************************************************************************/

MOVELIST* GenerateMoves(POSITION position)
{
	MOVELIST* moves = NULL;

	/* Use CreateMovelistNode(move, next) to 'cons' together a linked list */
	int player = generic_hash_turn(position);
	char* board = generic_hash_unhash(position, gBoard);
	int playerpiece = (player == 1 ? WHITE : BLACK); // 1 is white, 2 is black

	int i, c, r;
	BOOLEAN hasCapture = FALSE;
	for (i = 0; i < gBoardSize; i++) {
		if (board[i] == playerpiece) {
			c = getColumn(i);
			r = getRow(i);

			for (int dir = 0; dir < 8; dir++) {
				if (canMoveDir(i, dir, playerpiece) == 2) {
					moves = CreateMovelistNode(EncodeMove(dir, c, r, 1), moves);
					hasCapture = TRUE;
				}
			}
		}
	}

	//printf("\n   %c required capture? %c\n", playerpiece, hasCapture ? 'Y' : 'N');

	if (!hasCapture) {
		for (i = 0; i < gBoardSize; i++) {
			c = getColumn(i);
			r = getRow(i);

			if (board[i] == playerpiece) {
				if (isAtBoardEnd(i, playerpiece) && (fixedAtBoardEnd)) {
					continue;
				}
				for (int dir = 0; dir < 8; dir++) {
					if (canMoveDir(i, dir, playerpiece) == 1) {
						moves = CreateMovelistNode(EncodeMove(dir, c, r, 0), moves);
					}
				}
			}
		}
	}

	/*
	MOVE mv1 = moves->move;
	int dir = GetDirection(mv1);
	int x0 = GetXCoord(mv1);
	int y0 = GetYCoord(mv1);
	printf("move option 1: (%d, %d) -> %d\n", x0, y0, dir);
	int j = getArraynum(x0, y0);
	printf("board[%d] = %c", j, board[j]);
	*/

	return moves;
}


/************************************************************************
**
** NAME:        DoMove
**
** DESCRIPTION: Applies the move to the position.
**
** INPUTS:      POSITION position : The old position
**              MOVE     move     : The move to apply to the position
**
** OUTPUTS:     (POSITION)        : The position that results from move
**
** CALLS:       Some Board Hash Function
**              Some Board Unhash Function
**
*************************************************************************/

POSITION DoMove(POSITION position, MOVE move)
{
	int dir = GetDirection(move);
	int x0 = GetXCoord(move);
	int y0 = GetYCoord(move);
	int isJump = GetMoveIsJump(move);
	int x1, y1; // target destination

	char* board = generic_hash_unhash(position, gBoard);
	//int* board = unhash(position, gBoard);
	int player = generic_hash_turn(position);
	//int player = unhash_turn(position);
	int nextPlayer = -player + 3;

	//printf("\n player %d (%c) making move %d\n\n", 
	//	player, (player == 1) ? WHITE : BLACK, move);

	/* store new piece coords in x1 y1 */
	switch (dir) {
	case UP:
		x1 = x0;
		y1 = y0 + 1 + isJump;
		break;
	case RIGHT:
		x1 = x0 + 1 + isJump;
		y1 = y0;
		break;
	case LEFT:
		x1 = x0 - (1 + isJump);
		y1 = y0;
		break;
	case UPLEFT:
		x1 = x0 - (1 + isJump);
		y1 = y0 + (1 + isJump);
		break;
	case UPRIGHT:
		x1 = x0 + (1 + isJump);
		y1 = y0 + (1 + isJump);
		break;
	case DOWNRIGHT:
		x1 = x0 + (1 + isJump);
		y1 = y0 - (1 + isJump);
		break;
	case DOWN:
		x1 = x0;
		y1 = y0 - (1 + isJump);
		break;
	case DOWNLEFT:
		x1 = x0 - (1 + isJump);
		y1 = y0 - (1 + isJump);
		break;
	default:
		printf("Error: bad switch in DoMove\n");
		break;
	}

	/* convert x1 y1 to position index */
	int pIndex1 = getArraynum(x1, y1);

	/* get type of piece */
	int pIndex0 = getArraynum(x0, y0);
	char pType = board[pIndex0];

	/* doing basic movement */
	board[pIndex0] = BLANK;
	board[pIndex1] = pType;

	/* capture jumps */
	if (isJump) {
		int pIndex2 = getArraynum((x0 + x1) / 2, (y0 + y1) / 2); //index of jumped-over piece
		board[pIndex2] = BLANK;
	}

	//printf("\tMove (%d, %d) to (%d, %d). Is%s jump.\n\n", x0, y0, x1, y1, isJump ? "" : " not");

	return generic_hash_hash(board, nextPlayer);
	//long h = hash(board, nextPlayer);
	//return h;
}

/************************************************************************
**
** NAME:        Primitive
**
** DESCRIPTION: Returns the value of a position if it fulfills certain
**              'primitive' constraints.
**
**              Example: Tic-tac-toe - Last piece already placed
**
**              Case                                  Return Value
**              *********************************************************
**              Current player sees three in a row    lose
**              Entire board filled                   tie
**              All other cases                       undecided
**
** INPUTS:      POSITION position : The position to inspect.
**
** OUTPUTS:     (VALUE)           : one of
**                                  (win, lose, tie, undecided)
**
** CALLS:       None
**
************************************************************************/

VALUE Primitive(POSITION position)
{
	char* board = (char*)generic_hash_unhash(position, gBoard);
	//int* board = unhash(position, gBoard);
	int player = generic_hash_turn(position);
	//int player = unhash_turn(position);
	int playerpiece = (player == 1 ? WHITE : BLACK);
	int numplayerpiece = 0;
	for (int i = 0; i < gBoardSize; i++) {
		if (board[i] == playerpiece) {
			numplayerpiece++;
		}
	}

	if (numplayerpiece == 0) {
		//printf("Player %c loses: no pieces remaining.\n", playerpiece);
		return gStandardGame ? lose : win;
	}
	else if (GenerateMoves(position) == NULL) {
		//printf("Player %c loses: no possible moves.\n", playerpiece);
		return gStandardGame ? lose : win;
	}
	else
		return undecided;
}

/************************************************************************
**
** NAME:        PrintPosition
**
** DESCRIPTION: Prints the position in a pretty format, including the
**              prediction of the game's outcome.
**
** INPUTS:      POSITION position    : The position to pretty print.
**              STRING   playersName : The name of the player.
**              BOOLEAN  usersTurn   : TRUE <==> it's a user's turn.
**
** CALLS:       Unhash()
**              GetPrediction()      : Returns the prediction of the game
**
************************************************************************/

void PrintPosition(POSITION position, STRING playersName, BOOLEAN usersTurn)
{
	char* board = generic_hash_unhash(position, gBoard);
	//int* board = unhash(position, gBoard);
	int player = generic_hash_turn(position);
	//int player = unhash_turn(position);
	printf("Current position: %lld (%d)\n", position, player);

	STRING playercolor;
	if (player == 1) {
		playercolor = "WHITE";
	}
	else if (player == 2) {
		playercolor = "BLACK";
	}
	else {
		printf("Error: invalid player num = %d\n", player);
		playercolor = "";
	}
	printf("%s's turn (%s)\n", playersName, playercolor);

	/* counters */
	int h, i, j, k, c, c1;
	/* for diagonals */
	c = 1;
	printf("Board: ");
	for (h = 0; h < gBoardWidth - 1; h++) {
		printf("\t");
	}
	printf("Key: \n");
	for (i = 0; i < gBoardSize; ) {
		for (j = 0; j < gBoardWidth; j++) {
			printf("(%c)", board[i]);
			i++;
			if (j < gBoardWidth - 1) {
				printf("-");
			}
		}
		printf("\t\t");
		for (c1 = 0; c1 < gBoardWidth; c1++) {
			if (c < 10) {
				printf("( %d)", c);
				c++;
				if (c1 < gBoardWidth - 1) {
					printf("--");
				}
			}
			else {
				printf("(%d)", c);
				c++;
				if (c1 < gBoardWidth - 1) {
					printf("--");
				}
			}
		}
		if (i < gBoardSize - gBoardWidth + 1) {
			printf("\n");
			for (k = 0; k < gBoardWidth - 1; k++) {
				printf(" | %c", ((k % 2) ^ (i % 2) ? '\\' : '/'));
			}
			printf(" |");
			printf("\t\t");
			for (k = 0; k < gBoardWidth - 1; k++) {
				printf(" |    ");
			}
			printf(" |");
			printf("\t\t");
		}
		printf("\n");
	}
	printf("\n%s\n", GetPrediction(position, playersName, usersTurn));

}

/************************************************************************
**
** NAME:        PrintComputersMove
**
** DESCRIPTION: Nicely formats the computers move.
**
** INPUTS:      MOVE    computersMove : The computer's move.
**              STRING  computersName : The computer's name.
**
************************************************************************/

void PrintComputersMove(MOVE computersMove, STRING computersName)
{
	printf("%8s's move   : ", computersName);
	PrintMove(computersMove);
	printf("\n");
}

/************************************************************************
**
** NAME:        PrintMove
**
** DESCRIPTION: Prints the move in a nice format.
**
** INPUTS:      MOVE move         : The move to print.
**
************************************************************************/

void PrintMove(MOVE move)
{
	STRING m = MoveToString(move);
	printf("%s", m);
	SafeFree(m);
}

/************************************************************************
**
** NAME:        MoveToString
**
** DESCRIPTION: Returns the move as a STRING
**
** INPUTS:      MOVE *mv         : The move to put into a string.
**
************************************************************************/

STRING MoveToString(MOVE mv)
{
	STRING m = (STRING)SafeMalloc(8);
	int xcoord, ycoord, dir, Arraynum;
	STRING direction;
	xcoord = GetXCoord(mv);
	ycoord = GetYCoord(mv);
	dir = GetDirection(mv);
	Arraynum = getArraynum(xcoord, ycoord);
	if (dir == UP)
		direction = "u";
	else if (dir == RIGHT)
		direction = "r";
	else if (dir == LEFT)
		direction = "l";
	else if (dir == DOWN)
		direction = "d";
	else if (dir == UPLEFT)
		direction = "ul";
	else if (dir == UPRIGHT)
		direction = "ur";
	else if (dir == DOWNRIGHT)
		direction = "dr";
	else if (dir == DOWNLEFT)
		direction = "dl";
	sprintf(m, "[%d %s]", Arraynum + 1, direction);
	return m;
}


/************************************************************************
**
** NAME:        GetAndPrintPlayersMove
**
** DESCRIPTION: Finds out if the player wishes to undo, abort, or use
**              some other gamesman option. The gamesman core does
**              most of the work here.
**
** INPUTS:      POSITION position    : Current position
**              MOVE     *move       : The move to fill with user's move.
**              STRING   playersName : Current Player's Name
**
** OUTPUTS:     USERINPUT          : One of
**                                   (Undo, Abort, Continue)
**
** CALLS:       USERINPUT HandleDefaultTextInput(POSITION, MOVE*, STRING)
**                                 : Gamesman Core Input Handling
**
************************************************************************/

USERINPUT GetAndPrintPlayersMove(POSITION position, MOVE* move, STRING playersName)
{
	USERINPUT input;
	USERINPUT HandleDefaultTextInput();

	for (;; ) {
		/***********************************************************
		* CHANGE THE LINE BELOW TO MATCH YOUR MOVE FORMAT
		***********************************************************/
		printf("%8s's move [(undo)/(key dir [1 d])] : ", playersName);

		input = HandleDefaultTextInput(position, move, playersName);

		if (input != Continue)
			return input;
	}

	/* NOTREACHED */
	return Continue;
}


/************************************************************************
**
** NAME:        ValidTextInput
**
** DESCRIPTION: Rudimentary check to check if input is in the move form
**              you are expecting. Does not check if it is a valid move.
**              Only checks if it fits the move form.
**
**              Reserved Input Characters - DO NOT USE THESE ONE CHARACTER
**                                          COMMANDS IN YOUR GAME
**              ?, s, u, r, h, a, c, q
**                                          However, something like a3
**                                          is okay.
**
**              Example: Tic-tac-toe Move Format : Integer from 1 to 9
**                       Only integers between 1 to 9 are accepted
**                       regardless of board position.
**                       Moves will be checked by the core.
**
** INPUTS:      STRING input : The string input the user typed.
**
** OUTPUTS:     BOOLEAN      : TRUE if the input is a valid text input.
**
************************************************************************/

BOOLEAN ValidTextInput(STRING input)
{
	if (input[0] < '0' || input[0] > '9') {
		//printf("badinput[0]");
		return FALSE;
	}
	char* spcptr = strchr(input, ' ');
	if (spcptr == NULL) {
		//printf("badspcptr");
		return FALSE;
	}

	*spcptr = '\0';
	char* direction = ++spcptr;

	if (strcmp(direction, "u") && strcmp(direction, "r") &&
		strcmp(direction, "d") && strcmp(direction, "l") &&
		strcmp(direction, "ur") && strcmp(direction, "ul") &&
		strcmp(direction, "dr") && strcmp(direction, "dl")) {
		printf("badelse");
		return FALSE;
	}
	*(spcptr - 1) = ' ';
	return TRUE;

}


/************************************************************************
**
** NAME:        ConvertTextInputToMove
**
** DESCRIPTION: Converts the string input your internal move representation.
**              Gamesman already checked the move with ValidTextInput
**              and ValidMove.
**
** INPUTS:      STRING input : The VALID string input from the user.
**
** OUTPUTS:     MOVE         : Move converted from user input.
**
************************************************************************/

MOVE ConvertTextInputToMove(STRING input)
{
	char* spcptr = strchr(input, ' ');
	*spcptr = '\0';
	char* direction = ++spcptr;
	int location = atoi(input);
	location--;

	int dir;
	if (0 == strcmp(direction, "u")) {
		dir = UP;
	}
	else if (0 == strcmp(direction, "r")) {
		dir = RIGHT;
	}
	else if (0 == strcmp(direction, "l")) {
		dir = LEFT;
	}
	//for diagonals
	else if (0 == strcmp(direction, "ul")) {
		dir = UPLEFT;
	}
	else if (0 == strcmp(direction, "ur")) {
		dir = UPRIGHT;
	}
	else if (0 == strcmp(direction, "dl")) {
		dir = DOWNLEFT;
	}
	else if (0 == strcmp(direction, "d")) {
		dir = DOWN;
	}
	else if (0 == strcmp(direction, "dr")) {
		dir = DOWNRIGHT;
	}
	else {
		printf("bad else in ConvertTextInputToMove");
	}

	int x, y;
	x = getColumn(location);
	y = getRow(location);
	int j = canMoveDir(location, dir, gBoard[location]) - 1;
	//printf("Converted move: (%d, %d) in direction %d. Is%s jump.\n", x, y, dir, j==1 ? "" : " not");
	return EncodeMove(dir, x, y, j);
}


/************************************************************************
**
** NAME:        GameSpecificMenu
**
** DESCRIPTION: Prints, receives, and sets game-specific parameters.
**
**              Examples
**              Board Size, Board Type
**
**              If kGameSpecificMenu == FALSE
**                   Gamesman will not enable GameSpecificMenu
**                   Gamesman will not call this function
**
**              Resets gNumberOfPositions if necessary
**
************************************************************************/

void GameSpecificMenu()
{
	char GetMyChar();
	int intWidth;

	printf("\n");
	printf("Alquerque Game Specific Menu:\n");
	printf("\tw) \tChange the Board (W)idth. \n\t\t* Current Board Width: %d\n", gBoardWidth);
	printf("\tf) \tToggle (f)ixed piece at opposite board end, except for captures. \n\t\t* Currently: %s\n",
		fixedAtBoardEnd ? "ON" : "OFF");
	printf("\tb) \t(B)ack to previous menu\n");
	printf("\n\n\tq) \t(Q)uit\n");
	printf("Select an option: ");

	switch (GetMyChar()) {
	case 'q':
	case 'Q':
		printf("\n");
		ExitStageRight();
		break;
	case 'w':
	case 'W':
		printf("Input desired board width (must be an odd integer): ");
		intWidth = GetMyUInt();
		if (intWidth % 2 == 1) {
			gBoardWidth = intWidth;
			gBoardSize = gBoardWidth * gBoardWidth;
			InitializeGame();
		}
		else {
			printf("%d is not a valid board width. Width must be odd.\n", intWidth);
		}
		GameSpecificMenu();
		break;
	case 'f':
	case 'F':
		fixedAtBoardEnd = !fixedAtBoardEnd;
		GameSpecificMenu();
		break;
	case 'b':
	case 'B':
		return;
	default:
		printf("Not a valid option.\n");
		GameSpecificMenu();
		break;
	}

}


/************************************************************************
**
** NAME:        SetTclCGameSpecificOptions
**
** DESCRIPTION: Set the C game-specific options (called from Tcl)
**              Ignore if you don't care about Tcl for now.
**
************************************************************************/

void SetTclCGameSpecificOptions(int options[])
{

}


/************************************************************************
**
** NAME:        GetInitialPosition
**
** DESCRIPTION: Called when the user wishes to change the initial
**              position. Asks the user for an initial position.
**              Sets new user defined gInitialPosition and resets
**              gNumberOfPositions if necessary
**
** OUTPUTS:     POSITION : New Initial Position
**
************************************************************************/

POSITION GetInitialPosition()
{
	printf("Calling getInitialPosition in C");
	return gInitialPosition;
}


/************************************************************************
**
** NAME:        NumberOfOptions
**
** DESCRIPTION: Calculates and returns the number of variants
**              your game supports.
**
** OUTPUTS:     int : Number of Game Variants
**
************************************************************************/

int NumberOfOptions()
{
	return 4;
}


/************************************************************************
**
** NAME:        getOption
**
** DESCRIPTION: A hash function that returns a number corresponding
**              to the current variant of the game.
**              Each set of variants needs to have a different number.
**
** OUTPUTS:     int : the number representation of the options.
**
************************************************************************/

int getOption()
{
	int option = 1;
	if (gStandardGame)
		option = 1;
	else
		option = 0;
	option += gBoardWidth - 3;
	return option;

}


/************************************************************************
**
** NAME:        setOption
**
** DESCRIPTION: The corresponding unhash function for game variants.
**              Unhashes option and sets the necessary variants.
**
** INPUT:       int option : the number representation of the options.
**
************************************************************************/

void setOption(int option)
{
	gStandardGame = ((option & 1) == 1);
	gBoardWidth = (option >> 1) * 2 + 1;
}


/************************************************************************
**
** NAME:        DebugMenu
**
** DESCRIPTION: Game Specific Debug Menu (Gamesman comes with a default
**              debug menu). Menu used to debug internal problems.
**
**              If kDebugMenu == FALSE
**                   Gamesman will not display a debug menu option
**                   Gamesman will not call this function
**
************************************************************************/

void DebugMenu()
{

}


/************************************************************************
**
** Everything specific to this module goes below these lines.
**
** Things you want down here:
** Move Hasher
** Move Unhasher
** Any other function you deem necessary to help the ones above.
**
************************************************************************/

/* Simple base-3 hash, until I can figure out how to use generic_hash*/
/*
long hash(int* board, int turn) {
	long val = 0;
	for (int i = 0; i < gBoardSize; i++) {
		val += board[i] * (pow(3, i)); //msb last
	}
	val += turn * (pow(3, gBoardSize));
	return val;
}

int* unhash(long hash, int* dest) {
	for (int i = 0; i < gBoardSize; i++) {
		dest[i] = ((int)(hash / pow(3, i)) % 3);
	}
	return dest;
}

int unhash_turn(long hash) {
	return ((int)(hash / pow(3, gBoardSize)) % 3);
}
*/

/* MOVE stuff
   MOVES are hashed where leftmost three bits is x, second three is y, final is jump */
MOVE EncodeMove(int dir, int x, int y, int jump)
{
	dir = dir << 7;
	x = x << 4;
	y = y << 1;
	return (dir | x | y | jump);
}

int GetDirection(MOVE mv)
{
	return (mv >> 7);
}

int GetXCoord(MOVE mv)
{
	return (mv >> 4) & 0x7;
}

int GetYCoord(MOVE mv)
{
	return (mv >> 1) & 0x7;
}

int GetMoveIsJump(MOVE mv) {
	return mv & 0b1;
}


/* Helper Functions for Generate Moves
   All columns and rows start with zero and start from the lower left */
int getArraynum(int xcoord, int ycoord) {
	return ((gBoardWidth * (gBoardWidth - ycoord)) - (gBoardWidth - xcoord));
}

int getColumn(int arraynum)
{
	return (arraynum % gBoardWidth);
}

int getRow(int arraynum)
{
	return gBoardWidth - 1 - (arraynum / gBoardWidth);
}

int getTarget(int arraynum, int dir, int jump) {
	int vertical = gBoardWidth * (jump + 1);
	int horiz = 1 + jump;
	switch (dir) {
	case LEFT:
		return arraynum - horiz;
	case UPLEFT:
		return arraynum - horiz - vertical;
	case UP:
		return arraynum - vertical;
	case UPRIGHT:
		return arraynum + horiz - vertical;
	case RIGHT:
		return arraynum + horiz;
	case DOWNRIGHT:
		return arraynum + horiz + vertical;
	case DOWN:
		return arraynum + vertical;
	case DOWNLEFT:
		return arraynum - horiz + vertical;
	default:
		return -1;
	}
}

int canMoveDir(int arraynum, int dir, int current) {
	if (arraynum >= gBoardSize || arraynum < 0)
		return 0; //is this even necessary?

	if (current == WHITE && (dir == DOWN || dir == DOWNLEFT || dir == DOWNRIGHT)) {
		return 0; //white can't move down
	}

	if (current == BLACK && (dir == UP || dir == UPLEFT || dir == UPRIGHT)) {
		return 0; //black can't move up
	}

	if (arraynum % 2 && dir % 2) { // odd index doesn't have diagonals (odd dirs)
		return 0;
	}

	int adj = getTarget(arraynum, dir, 0);
	int jumptarg = getTarget(arraynum, dir, 1);
	int x = arraynum % gBoardWidth;

	if (adj >= gBoardSize || adj < 0) {
		return 0;
	}

	BOOLEAN normalMove = (gBoard[adj] == BLANK);
	if (!normalMove && (jumptarg >= gBoardSize || jumptarg < 0)) {
		return 0; //if jump out of range AND normal move impossible
	}

	char opponent = (current == WHITE) ? BLACK : WHITE;
	BOOLEAN jumpMove = (gBoard[adj] == opponent) && (gBoard[jumptarg] == BLANK);
	int moveType = normalMove ? 1 : (jumpMove ? 2 : 0);

	// todo: for debugging certain positions
	/*int t = 0;
	if (arraynum == t) {
		printf(" board[%d] = %c", t, gBoard[t]);
		if (dir == DOWNRIGHT) {
			printf("\n curr pos: %d (%c)\n", arraynum, gBoard[arraynum]);
			printf(" adj: %d (%c)\n", adj, gBoard[adj]);
			//printf(" jumptarg: %d (%s)\n\n", adj, gBoard[adj] == BLANK ? "blank" : "not blank");
			printf(" movetype: %d", moveType);
		}
	}*/

	switch (dir) {
	case LEFT:
	case DOWNLEFT:
	case UPLEFT:
		if (x < moveType) {
			return 0;
		}
		break;
	case RIGHT:
	case UPRIGHT:
	case DOWNRIGHT:
		if (gBoardWidth - x <= moveType) {
			return 0;
		}
	default:
		break;
	}
	return moveType; //normal = 1, jump = 2
}

int isAtBoardEnd(int arraynum, int player) {
	return (player == WHITE && arraynum < gBoardWidth) || (player == BLACK && arraynum > (gBoardSize - gBoardWidth));
}

POSITION StringToPosition(char* board) {
	return -1;
}

char* PositionToString(POSITION pos) {
	// FIXME: this is just a stub
	return "Implement Me";
}

char* PositionToEndData(POSITION pos) {
	return NULL;
}
