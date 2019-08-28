/*
 * Anon 19X: Kyle Bensink, Ashley Song, Rylee Stone, Barett Noone
 *
 * movment.h - an API for different movement heuristics we have come up with.
 * Each avatar passes in the requesite information and the movement function
 * determines what the avatar's next move will be, and returns the cardinal
 * direction to go next. This API holds much of the integral logic of the maze
 * solving algorithm.
 *
*/

#include "maze.h"
#include "common.h"

/******************** move_random() ********************/
/*
This function generates a random direction to move, sees if said move
is valid, and returns that direction if it is.
A valid move is defined by: the maze struct does not have a wall where
the avatar is trying to move. Either no wall exists there or it has
not yet been explored.

NOTE: This function was used in the early stages of development. The final program
      does not use this function.
*/
int move_random(maze_t *maze, XYPos *current);

/******************** move_wallFollower() ********************/
/*
This function holds the key logic of the maze solving algorithm. Essentially,
the avatar is following the maze, blind, by placing its "right hand" on the wall
and following that wall until the maze is "solved".

The previous direction is important in deciding what direction
to turn with the wall. In short, if the wall has a turn to the right, the avatar will
turn right with it. Otherwise, the avatar will progressively turn left until a valid move

Input: maze_t struct, current XY position of the avatar, and the direction the avatar
      moved last time.
Output: the new direction avatar should go, in integer representation of the cardinal movements
        defined in common.h
*/
int move_wallFollower(maze_t *maze, XYPos *current, int inputDirection);
