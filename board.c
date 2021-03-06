#include "board.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/****** chefarov's functions' ******/

void printmove(Move m){
	printf("[%d,%d]->[%d,%d] \n", m.tile[0][0], m.tile[1][0], m.tile[0][1], m.tile[1][1]);
}

void setmove(Move * move, int startRow, int startCol, int endRow, int endCol, int turn){
	
	if(move == NULL)	
		move = (Move*)malloc(sizeof(Move*));
		
	move->tile[0][0] = startRow;
	move->tile[0][1] = endRow;
	move->tile[1][0] = startCol;
	move->tile[1][1] = endCol;
	move->tile[0][2] = -1;  //move ends here	

	move->color = turn;
}

/* if move has already been setted then append new (endRow, endCol) destination */
void appendmove(Move * move, int startRow, int startCol, int endRow, int endCol, int turn){
	int i;
	//if move empty then just setmove
	if(move == NULL || move->tile[0][0] != startRow){
		setmove(move, startRow, startCol, endRow, endCol, turn);
		return;
	}
	//else : search to find where the last move reached to append the new ending coords
	for(i=2; i<MAXIMUM_MOVE_SIZE; i++){
		if(move->tile[0][i] == -1){     //no more coords
			move->tile[0][i] = endRow;
			move->tile[1][i] = endCol;
			if(i<MAXIMUM_MOVE_SIZE-1)
				move->tile[0][i+1] = -1;	//helping for appendmove ()
		}
	}
}


int getPossibleMoves(Position * curPosition, Move moves[24]){

	int i,j,k=0, jumps;
	Move tmpMove;
	Move possibleJumps[4];	//cant have more each time
	
	jumps = findJumps(curPosition, possibleJumps);
	if( jumps > 0){	     //found at least one jump return the jump moves
		while (jumps > k){
			copy_move(&moves[k], &possibleJumps[k]);
			k++;
		}		
		return jumps;		//since the game predicts that jumps are played in any case 
	}		
		
	/* Possible optimization: change iteration direction depending myColor */
	for(i=0; i<BOARD_SIZE; i++){
		for(j=0; j<BOARD_SIZE; j++){
			if(curPosition -> board[i][j] == curPosition->turn){				
				if(curPosition->turn == WHITE){			// WHITE PLAYER
					//try move to the right
					setmove(&tmpMove, i,j,i+1,j+1,curPosition->turn);
					if(isLegal(curPosition, &tmpMove))
						copy_move(&moves[k++], &tmpMove);
					//try move to the left
					setmove(&tmpMove, i,j,i+1,j-1,curPosition->turn);
					if(isLegal(curPosition, &tmpMove))
						copy_move(&moves[k++], &tmpMove);
				}else{									// BLACK PLAYER
					//try move to the right
					setmove(&tmpMove, i,j,i-1,j+1, curPosition->turn);
					if(isLegal(curPosition, &tmpMove))
						copy_move(&moves[k++], &tmpMove);
					//try move to the left
					setmove(&tmpMove, i,j,i-1,j-1, curPosition->turn );
					if(isLegal(curPosition, &tmpMove))
						copy_move(&moves[k++], &tmpMove);
				}
			}
		}
	}
	return k;	//how many moves found		
}


void copy_position(Position *dest, Position *src){
	int i,j;
	
	if(dest == NULL)
		dest = (Position*)malloc(sizeof(Position*));

	memcpy(dest->board[i], src->board[i], BOARD_SIZE*BOARD_SIZE);
			
	dest->score[0] = src->score[0]; dest->score[1] = src->score[1];
	dest->turn = src->turn;
	//dest->ourPieces = src->ourPieces; dest->opponentPieces = src->opponentPieces;
}

void copy_move(Move *dest, Move *src){
	int i,j;
	
	if(dest == NULL)
		dest = (Move*)malloc(sizeof(Move*));
		
	memcpy(dest->tile, src->tile, 2*MAXIMUM_MOVE_SIZE);

	dest->color = src->color;
}

//if any jump possible for us
int canWeJump(Position *pos){
	int i,j;
	
	for( i = 0; i < BOARD_SIZE; i++ )
		for( j = 0; j < BOARD_SIZE; j++ )
			if( pos->board[ i ][ j ] == myColor )
				if( canJump( i, j, myColor, pos ) )
					return 1;		//there is
					
	return 0;		//there isnt
}

int findJumps (Position * pos, Move * possibleJumps){

	int k, i,j, playerDirection, c=0, dir=0, i2,j2;

	if( pos->turn == WHITE )		// find movement's direction
		playerDirection = 1;
	else
		playerDirection = -1;

//	printf("going to find jumps for %d\n", pos->turn);
	for( i = 0; i < BOARD_SIZE; i++ ){
		for( j = 0; j < BOARD_SIZE; j++ ){
			if( pos->board[ i ][ j ] == pos->turn ){
				dir = canJump(i, j, pos->turn, pos);
				if(dir != 0){	//we have at least a jump
					k=1;
					i2=i; j2=j;
					do
					{
						if(k==1){	//first jump
							possibleJumps[c].tile[0][0] = i2;
							possibleJumps[c].tile[1][0] = j2;
						}
						possibleJumps[c].tile[ 0 ][ k ] = i2 + 2 * playerDirection;
					
						if( dir == 1 ) //left jump possible
							possibleJumps[c].tile[ 1 ][ k ] = j2 - 2;
						else if(dir == 2) //righ jump possible
							possibleJumps[c].tile[ 1 ][ k ] = j2 + 2;
						else if(dir == 3){ //both jumps possibleJumps 
							possibleJumps[c].tile[ 1 ][ k ] = j2 - 2;
						//	printf("jumping left instead of right \n");
							//possibleJumps[++c].tile[ 1 ][ k ] = j + 2;
							//i = myMove.tile[ 0 ][ k ];		//we will try to jump from this point in the next loop
						//	j = myMove.tile[ 1 ][ k ];
							//k++;
						}	
						if( k + 1 == MAXIMUM_MOVE_SIZE )	//maximum tiles reached
							break;
						possibleJumps[c].tile[ 0 ][ k + 1 ] = -1;		//maximum tiles not reached
						possibleJumps[c].color = pos->turn;

						i2 = possibleJumps[c].tile[ 0 ][ k ];		//we will try to jump from this point in the next loop
						j2 = possibleJumps[c].tile[ 1 ][ k ];
						k++;
						
					}while( (dir = canJump(i2, j2, pos->turn, pos))  != 0 );
					c++; //printf("c=%d\n", c);
			   	}
			}
		}
	}
	return c;
}


/********* Giannis functions'    *********/

void initPosition( Position * pos )
{
	int i, j;
	int rowsForEachPlayer;

	/* Calculate how many rows will be filled with pieces for each player */
	rowsForEachPlayer = ( BOARD_SIZE - 2 ) / 2;

	for( i = 0; i < BOARD_SIZE; i++ )
	{
		for( j = 0; j < BOARD_SIZE; j++ )
	{
			pos->board[ i ][ j ] = EMPTY;	// first empty every square

		if( ( i + j ) % 2 == 1 )	// if ( i + j = odd ) then place piece
			{
				if( i < rowsForEachPlayer )
					pos->board[ i ][ j ] = WHITE;
				else if( i >= BOARD_SIZE - rowsForEachPlayer )
					pos->board[ i ][ j ] = BLACK;
			}

		}
	}

	/* Reset score */
	pos->score[ WHITE ] = 0;
	pos->score[ BLACK ] = 0;
	
	/* Black plays first */
	pos->turn = BLACK;
}


/**********************************************************/
void printBoard( char board[ BOARD_SIZE ][ BOARD_SIZE ] )
{
	int i, j;

	/* Print the upper section */
	printf( "   " );
	for( i = 0; i < BOARD_SIZE; i++ )
		printf( "%d ", i );
	printf( "\n +" );
	for( i = 0; i < 2 * BOARD_SIZE + 1; i++ )
		printf( "-" );
	printf( "+\n" );

	/* Print board */
	for( i = 0; i < BOARD_SIZE; i++ )
	{
		printf( "%d| ", i );
		for( j = 0; j < BOARD_SIZE; j++ )
			switch( board[ i ][ j ] )
			{
				case WHITE:
					printf( "W " );
					break;
				case BLACK:
					printf( "B " );
					break;
				case EMPTY:
					printf( ". " );
					break;
				case ILLEGAL:
					printf( "# " );
					break;
				default:
					printf("ERROR: Unknown character in board (printBoard)\n");
					exit( 1 );
			}
		printf ( "|%d\n", i );
	}

	/* Print the lower section */
	printf( " +" );
	for( i = 0; i < 2 * BOARD_SIZE + 1; i++ )
		printf( "-" );
	printf( "+\n" );
	printf( "   " );
	for( i = 0; i < BOARD_SIZE; i++ )
		printf( "%d ", i );
	printf( "\n" );

}


/**********************************************************/
void printPosition( Position * pos )
{
	//board
	printBoard( pos->board );

	//turn
	if( pos->turn == WHITE )
		printf( "Turn: WHITE" );
	else if( pos->turn == BLACK )
		printf( "Turn: BLACK" );
	else
		printf( "Turn: -" );

	printf( "\n" );

	//score
	printf( "Score is  W: %d  B: %d\n" , pos->score[ WHITE ], pos->score[ BLACK ] );
	//pieces
	//printf("Our pieces %d, opponent Pieces: %d \n", pos->ourPieces, pos->opponentPieces); 
}


/**********************************************************/
void doMove( Position * pos, Move * moveToDo )
{
	int i, j;
	int intex = 1;
	int stepI, stepJ;

	assert( pos->turn == moveToDo->color );

	//printf("making move "); printmove(*moveToDo);
	
	if( moveToDo->tile[ 0 ][ 0 ] == -1 )	//if null move, then simply change turn
	{
		pos->turn = getOtherSide( pos->turn );
		return;
	}

	while( moveToDo->tile[ 0 ][ intex ] != -1 )		//while we have tile available
	{
		pos->board[ moveToDo->tile[ 0 ][ intex - 1 ] ][ moveToDo->tile[ 1 ][ intex - 1 ] ] = EMPTY;	//remove piece

		if( abs( moveToDo->tile[ 0 ][ intex - 1 ] - moveToDo->tile[ 0 ][ intex ] ) > 1 )	//if we had jump
		{
			stepI = ( moveToDo->tile[ 0 ][ intex ] - moveToDo->tile[ 0 ][ intex - 1 ] ) / 2;
			stepJ = ( moveToDo->tile[ 1 ][ intex ] - moveToDo->tile[ 1 ][ intex - 1 ] ) / 2;

			pos->board[ moveToDo->tile[ 0 ][ intex - 1 ] + stepI ][ moveToDo->tile[ 1 ][ intex - 1 ] + stepJ ] = EMPTY;	//remove the captured piece

			/*<code added by chefarov> to keep track of the number of pawns - cpu optimization
			//a piece has been captured so reduce the appropriate counter
			if(pos->turn == myColor)
				pos->opponentPieces--;
			else
				pos->ourPieces--;
			/*</chefarov's code ended> */
		}

		if( moveToDo->tile[ 0 ][ intex ] == 0 )
			pos->score[ BLACK ]++;	//Black scored!
		else if( moveToDo->tile[ 0 ][ intex ] == BOARD_SIZE - 1 )
			pos->score[ WHITE ]++;	//White scored!
		else
			pos->board[ moveToDo->tile[ 0 ][ intex ] ][ moveToDo->tile[ 1 ][ intex ] ] = moveToDo->color;	//place piece

		intex++;

		if( intex == MAXIMUM_MOVE_SIZE )	// if all move tiles used
			break;

	}

	/*change turn*/
	pos->turn = getOtherSide( pos->turn );

}


/**********************************************************/
int canJump( char row, char col, char player, Position * pos )
{
	int returnValue = 0;

	assert( ( player ==  WHITE ) || ( player == BLACK ) );
	assert( row >= 0 && row < BOARD_SIZE );
	assert( col >= 0 && col < BOARD_SIZE );

	if( player == WHITE )	//white player
	{
		if( row + 2 < BOARD_SIZE )
		{
			if( col - 2 >= 0 )
				if( pos->board[ row + 1][ col - 1] == BLACK && pos->board[ row + 2][ col - 2] == EMPTY )
					returnValue = returnValue + 1;	//left jump possible
			if( col + 2 < BOARD_SIZE )
				if( pos->board[ row + 1][ col + 1] == BLACK && pos->board[ row + 2][ col + 2] == EMPTY )
					returnValue = returnValue + 2;	//right jump possible
		}
	}
	else	//black player
	{
		if( row - 2 >= 0 )
		{
			if( col - 2 >= 0 )
			{
				if( pos->board[ row - 1][ col - 1] == WHITE && pos->board[ row - 2][ col - 2] == EMPTY )
				{
					returnValue = returnValue + 1;	//left jump possible
				}
			}

			if( col + 2 < BOARD_SIZE )
			{
				if( pos->board[ row - 1][ col + 1] == WHITE && pos->board[ row - 2][ col + 2] == EMPTY )
				{
					returnValue = returnValue + 2;	//right jump possible
				}
			}

		}
	}

	return returnValue;

}

/**********************************************************/
int canJumpTo( char row, char col, char player, Position * pos, char rowDest, char colDest )
{

	int playerDirection;
	int jumpDirection;

	assert( ( player ==  WHITE ) || ( player == BLACK ) );
	assert( row >= 0 && row < BOARD_SIZE );
	assert( col >= 0 && col < BOARD_SIZE );
	assert( rowDest >= 0 && rowDest < BOARD_SIZE );
	assert( colDest >= 0 && colDest < BOARD_SIZE );


	jumpDirection = canJump( row, col, player, pos );

	if( jumpDirection == 0 )
		return FALSE;

	if( player == WHITE )
		playerDirection = 1;
	else
		playerDirection = -1;


	if( ( jumpDirection % 2 ) == 1 )	//if left jump possible
	{
		if( ( row + 2 * playerDirection == rowDest ) && ( col - 2 == colDest ) )
			return TRUE;
	}

	if( jumpDirection > 1 )	//if right jump possible
	{
		if( ( row + 2 * playerDirection == rowDest ) && ( col + 2 == colDest ) )
			return TRUE;
	}

	return FALSE;

}


/**********************************************************/
int canMove( Position * pos, char player )
{
	int i, j;

	assert( ( player ==  WHITE ) || ( player == BLACK ) );

	for( i = 0; i < BOARD_SIZE; i++ )
	{
		for( j = 0; j < BOARD_SIZE; j++ )
		{
			if( pos->board[ i ][ j ] == player )		//when we find a piece of ours
			{
				if( player == WHITE )	//white color
				{
					if( i + 1 < BOARD_SIZE )
					{
						if( j - 1 >= 0 )
							if( pos->board[ i + 1 ][ j - 1 ] == EMPTY )		//check if we can move to the left
								return TRUE;
						if( j + 1 < BOARD_SIZE )
							if( pos->board[ i + 1 ][ j + 1 ] == EMPTY )		//check if we can move to the right
								return TRUE;
						if( canJump( i, j, WHITE, pos ) != 0 )			//check if we can make a jump
							return TRUE;
					}
				}
				else		//black color
				{
					if( i - 1 >= 0 )
					{
						if( j - 1 >= 0 )
							if( pos->board[ i - 1 ][ j - 1 ] == EMPTY )		//check if we can move to the left
								return TRUE;
						if( j + 1 < BOARD_SIZE )
							if( pos->board[ i - 1 ][ j + 1 ] == EMPTY )		//check if we can move to the right
								return TRUE;
						if( canJump( i, j, BLACK, pos ) != 0 )			//check if we can make a jump
							return TRUE;
					}
				}
			}
		}
	}

	return FALSE;

}

/**********************************************************/
int isLegal( Position * pos, Move * moveToCheck )
{
	int i, j;
	int intex;

	if( !canMove( pos, moveToCheck->color ) )	//if that player cannot move, the only legal move is null
	{
		if( moveToCheck->tile[ 0 ][ 0 ] == -1 )
			return TRUE;
		else
			return FALSE;
	}

	if( moveToCheck->tile[ 0 ][ 0 ] == -1 )	//since we can move, null move is illegal
		return FALSE;


	/* first coordinates must be inside board */
	if( ( moveToCheck->tile[ 0 ][ 0 ] < 0 ) || ( moveToCheck->tile[ 0 ][ 0 ] >= BOARD_SIZE ) )
		return FALSE;
	if( ( moveToCheck->tile[ 1 ][ 0 ] < 0 ) || ( moveToCheck->tile[ 1 ][ 0 ] >= BOARD_SIZE ) )
		return FALSE;

	/* piece must be ours */
	if( pos->board[ moveToCheck->tile[ 0 ][ 0 ] ][ moveToCheck->tile[ 1 ][ 0 ] ] != moveToCheck->color )
		return FALSE;

	/* second coordinates must be inside board */
	if( ( moveToCheck->tile[ 0 ][ 1 ] < 0 ) || ( moveToCheck->tile[ 0 ][ 1 ] >= BOARD_SIZE ) )
		return FALSE;
	if( ( moveToCheck->tile[ 1 ][ 1 ] < 0 ) || ( moveToCheck->tile[ 1 ][ 1 ] >= BOARD_SIZE ) )
		return FALSE;

	/* square must be empty */
	if( pos->board[ moveToCheck->tile[ 0 ][ 1 ] ][ moveToCheck->tile[ 1 ][ 1 ] ] != EMPTY )
		return FALSE;

	/* if it seems to be a jump */
	if( ( abs( moveToCheck->tile[ 0 ][ 0 ] - moveToCheck->tile[ 0 ][ 1 ] ) == 2 ) && ( abs( moveToCheck->tile[ 1 ][ 0 ] - moveToCheck->tile[ 1 ][ 1 ] ) == 2 ) )
	{
		intex = 1;

		while( 1 )
		{
			if( !canJumpTo( moveToCheck->tile[ 0 ][ intex - 1 ], moveToCheck->tile[ 1 ][ intex -1 ], moveToCheck->color, pos, moveToCheck->tile[ 0 ][ intex ], moveToCheck->tile[ 1 ][ intex ] ) )
				return FALSE;

			/*if we reached all possible jumps for this board*/
			if( intex + 1 == MAXIMUM_MOVE_SIZE )
				return TRUE;

			/*no more jumps - we expect -1 to next row*/
			if( !canJump( moveToCheck->tile[ 0 ][ intex ], moveToCheck->tile[ 1 ][ intex ], moveToCheck->color, pos ) )
			{
				if( moveToCheck->tile[ 0 ][ intex + 1 ] == -1 )
					return TRUE;
				else
					return FALSE;
			}

			/* coordinates must be inside board - and NOT a -1 move */
			if( ( moveToCheck->tile[ 0 ][ intex + 1 ] < 0 ) || ( moveToCheck->tile[ 0 ][ intex + 1 ] >= BOARD_SIZE ) )
				return FALSE;
			if( ( moveToCheck->tile[ 1 ][ intex + 1 ] < 0 ) || ( moveToCheck->tile[ 1 ][ intex + 1 ] >= BOARD_SIZE ) )
				return FALSE;

			intex++;

		}

	}/* else if it seems to be a no-jump move */
	else if( ( abs( moveToCheck->tile[ 0 ][ 0 ] - moveToCheck->tile[ 0 ][ 1 ] ) == 1 ) && ( abs( moveToCheck->tile[ 1 ][ 0 ] - moveToCheck->tile[ 1 ][ 1 ] ) == 1 ) )
	{
		/*check direction*/
		if( ( moveToCheck->color == WHITE ) && ( moveToCheck->tile[ 0 ][ 1 ] - moveToCheck->tile[ 0 ][ 0 ] ) != 1 )
			return FALSE;
		if( ( moveToCheck->color == BLACK ) && ( moveToCheck->tile[ 0 ][ 1 ] - moveToCheck->tile[ 0 ][ 0 ] ) != -1 )
			return FALSE;

		/*seems legal..but we cannot be sure until we check if any jump is available*/
		for( i = 0; i < BOARD_SIZE; i++ )
		{
			for( j = 0; j < BOARD_SIZE; j++ )
			{
				if( pos->board[ i ][ j ] == moveToCheck->color )
					if( canJump( i, j, moveToCheck->color, pos ) )
						return FALSE;
			}
		}

		if( moveToCheck->tile[ 0 ][ 2 ] == -1 )
			return TRUE;


	}

	return FALSE;

}








