/*
 * movment.c - different functions that control movement
 *
 * for descriptions see movement.h
 *
 * Project Anon 19X
*/

#include "maze.h"
#include "movement.h"
// #include "common.h"

int move_random(maze_t *maze, XYPos *current)
{
  if (maze == NULL || current == NULL) return -1; // bad params

  XYPos *next = malloc(sizeof(XYPos));
  bool flag = true;
  int toReturn = -1;
  while (flag) {
    int direction = rand()%4;
    switch(direction) {
      case 0: //north
        next->x = current->x;
        next->y = current->y-1;
        // next = point_new(current->x,current->y-1);
        toReturn = M_NORTH;
        break;
      case 1: //south
        next->x = current->x;
        next->y = current->y+1;
        // next = point_new(current->x,current->y+1);
        toReturn = M_SOUTH;
        break;
      case 2: //east
        next->x = current->x+1;
        next->y = current->y;
        // next = point_new(current->x+1,current->y);
        toReturn = M_EAST;
        break;
      case 3: //west
        next->x = current->x-1;
        next->y = current->y;
        // next = point_new(current->x-1,current->y);
        toReturn = M_WEST;
        break;
      default:
        break;
    }
    if (next == NULL) return -1; //idk what's going on if this happens
    if (isValidMove(maze,current,next)) {
      flag = false;
    } else free(next);
  }
  free(next);
  return toReturn;
}

int move_wallFollower(maze_t *maze, XYPos *current, int inputDirection)
{
  if (maze == NULL || current == NULL) return -1; // bad params

  XYPos *next = malloc(sizeof(XYPos));
  bool flag = true;
  int toReturn = inputDirection;
  int direction = 0;
  int numTurns = 0;
  //parse inputDirection
  //turn right from the direction we went last time
  switch(inputDirection){
    case M_NORTH:
      direction = 1; //east
      break;
    case M_SOUTH:
      direction = 3; //west
      break;
    case M_EAST:
      direction = 2; //south
      break;
    case M_WEST:
      direction = 0; //north
      break;
    default:
      printf("bad inputDirection %d",inputDirection);
      return -1; //bad inputDirection
  }
  while (flag) {
    if (numTurns >5){
        // printf("esketit");
        break;
    }
    switch(direction) {
      case 0: //north
        next->x = current->x;
        next->y = current->y-1;
        // next = point_new(current->x,current->y-1);
        toReturn = M_NORTH;
        break;
      case 2: //south
        next->x = current->x;
        next->y = current->y+1;
        // next = point_new(current->x,current->y+1);
        toReturn = M_SOUTH;
        break;
      case 1: //east
        next->x = current->x+1;
        next->y = current->y;
        // next = point_new(current->x+1,current->y);
        toReturn = M_EAST;
        break;
      case 3: //west
        next->x = current->x-1;
        next->y = current->y;
        // next = point_new(current->x-1,current->y);
        toReturn = M_WEST;
        break;
      default:
        next = NULL;
        break;
    }
    if (next == NULL){
        printf("something is wrong");
        return -1;
    }
    if (isValidMove(maze,current,next)) {
      flag = false;
    } else {
      numTurns++;
      direction = (direction-1)%4; //turn left
      if (direction == -1) direction = 3;
      // printf("turning %d\n",direction);
    }
  }
  free(next);
  return toReturn;

}
