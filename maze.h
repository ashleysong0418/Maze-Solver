/*
 * Anon 19X: Kyle Bensink, Ashley Song, Rylee Stone, Barett Noone
 *
 * maze.h - API for tracking the maze as avatars are exploring
 *
 * Uses maze and mazePiece structs to create a 2D array that holds the information
 * about the boundaries of the maze, the walls and dead ends discovered.
 * Some of the functions are used in checking the maze to help the avatar decide
 * its next move.
 *
 * Methods for creating/deleting a maze, adding "discovered" walls,
 * and determining whether a move is valid
 *
 *
*/

#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include "common.h"
#include "./libcs50/memory.h"
// #include "./avatar.h"

/**************Structures***************/
typedef struct maze maze_t;
typedef struct mazePiece square_t;



/******************* maze_new() *******************/
/*
Creates a maze object for the given width and height.
The maze consists of mazePiece objects with wall values,
one for each square in the maze. The values correspond to
whether or not a wall has been found there.

Input: width and height of maze
Output: a maze_t struct if succesful. NULL if malloc fails to allocate space
*/
maze_t * maze_new(int width, int height);

/******************* isValidMove() *******************/
/*
Checks to see if the working maze already has a wall
where player wants to move. Returns, from past
experience, whether the avatar will hit a wall.
We assume the move is exactly one step in a cardinal direction
(no diagonal nonsense).

Input: a maze_t struct, current position of avatar in XYPos struct, and
      the position the avatar is attempting to move to
Output: true the move is valid, false if there is a wall there or if it is at the
        outer walls of the maze
*/
bool isValidMove(maze_t *maze, XYPos *moveFrom, XYPos *moveTo);

/******************* isDeadEnd() *******************/
/*
Checks to see if the previous location of the avatar is a dead end
so that a wall can be created. Returns true is there are walls in
the three other directions different from the avatar's movement.

Input: maze_t struct, the previous position of the avatar, and the position
      it is at right now after moving
Output: true if it is a dead end, false if it is not
*/

bool isDeadEnd(maze_t* maze, XYPos *lastPos, XYPos *currPos);

/******************* newWall() *******************/
/*
Creates a new wall if the server says a move is invalid.
Again, we assume the move is exactly one step in a cardinal direction.

Input: maze_t struct, location avatar is at, and the location is tried to move to
Output: no output, but will mark the wall in the maze_t struct pieces if it was a wall
        otherwise, it does not do anything
*/
void newWall(maze_t *maze, XYPos *moveFrom, XYPos *moveTo);

/******************* newDeadEndWall() *******************/
/*
Creates a "dead end wall", which prevents avatars from entering a
dead end without trapping avatars already in the dead end.

Input: a maze_t struct, current position of avatar, position avatar attempted to move to
Output: no output, it will mark a wall to block off the dead-end in the maze_t struct
        otherwise, it will do nothing
*/
void newDeadEndWall(maze_t *maze, XYPos *moveFrom, XYPos *moveTo);

/******************* printMaze() *******************/
/*
Prints out the maze to stdout.
This is mostly for debugging purposes.
Does not print out the location of each avatar.

Input: maze_t struct
Output: prints the maze at the point in the program
*/
void printMaze(maze_t *maze);

/******************* maze_delete() *******************/
/*
Deletes the maze_t struct before program quits. Clean-up

Input: maze_t struct
Output: nothing, just the freeing of space allocated for the maze_t struct
*/
void maze_delete(maze_t *maze);

/******************* beenVisited() *******************/
/*
Returns whether a square on the board has been visited

Input: maze_t struct, x and y position that is being checked
Output: true if visited, false if not
*/
bool beenVisited(maze_t *maze, int x, int y);

/******************* writeToFile() *******************/
/*
Prints the contents of the maze to a file for the GUI to read, prints in 0's and 1's

Input: maze_t struct, a file pointer to the maze log
Output: prints to the file to log
*/
void writeToFile(maze_t *maze, FILE *fp);
