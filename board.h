#ifndef _BOARD_H
#define _BOARD_H

#include "global.h"
#include "move.h"
/**********************************************************/

/* Position struct to store board, score and player's turn */
typedef struct
{
	char board[ BOARD_SIZE ][ BOARD_SIZE ];
	char score[ 2 ];
	char turn;
	//int ourPieces;
	//int opponentPieces;
} Position;


/**********************************************************/

/* Chefarov's functions */
void printmove(Move m);
//prints (one line) the move

int getPossibleMoves(Position * curPosition, Move moves[24]);
	
void copy_position(Position *dest, Position *src);
//copies src to dest

void copy_move(Move *dest, Move *src);
//copies src to dest

void setmove(Move * move, int startRow, int startCol, int endRow, int endCol, int turn);
//fills move's tile with coords

void appendmove(Move * move, int startRow, int startCol, int endRow, int endCol, int turn);
//if move has already been setted then append new (endRow, endCol) destination
	
int isJumpPossible(Position *pos);
//looks if anywhere onboard there is a jump

int findJumps (Position * pos, Move * possibleJumps);

/* Giannis functions' */
	
void initPosition( Position * pos );
//initializes position

void printBoard( char board[ BOARD_SIZE ][ BOARD_SIZE ] );
//prints board

void printPosition( Position * pos );
// Prints board along with Player's turn and score

void doMove( Position * pos, Move * moveToDo );
//plays moveToDo on position pos
//Caution!!! does not check if it is legal! Simply does the move!

int canJump( char row, char col, char player, Position * pos );
// returns 1 if we can jump to the left 2 if we can jump to the right 3 if we can jump both directions and 0 if no jump is possible
// row,col can be empty. So it can also be used to determine if we can make a jump from a position we do not occupy
//Caution!!! does no checks if we are inside the board

int canJumpTo( char row, char col, char player, Position * pos, char rowDest, char colDest );
// like canJump() it doesn't need row, col to be occupied by a piece.
// Caution!!! does no checks if we are inside board

int canMove( Position * pos, char player );
// determines if player can move

int isLegal( Position * pos, Move * moveToCheck );
// determines if a move is leagal

#endif
