/*
 * maze.c - a file for tracking a maze
 *
 * for function descriptions see maze.h
 *
 * Project Anon 19X
*/

#include "maze.h"
#include "common.h"


/**************Structures***************/
typedef struct maze {
  int width, height;
  struct mazePiece ***pieces;
} maze_t;

typedef struct mazePiece {
  bool north,south,east,west;
  bool visited;
} square_t;

/**************** static functions ****************/
// instantiates a new maze piece (represents one square in the maze
// with four possible moves - north south west east)
static square_t *mazePiece_new(int x, int y, int width, int height);


maze_t * maze_new(int width, int height)
{
  if (width <= 0 || height <= 0) return NULL;
  maze_t *maze = malloc(sizeof(maze_t));
  if (maze == NULL) return NULL; //memory error
  //set the instance variables
  maze->width = width;
  maze->height = height;
  maze->pieces = calloc(width,sizeof(square_t**));
  for (int i = 0; i < width; i++)
    maze->pieces[i] = calloc(height,sizeof(square_t*));
  //build array
  for (int i = 0; i < width; i++) {
    for (int j = 0; j < height; j++) {
      maze->pieces[i][j] = mazePiece_new(i,j,width,height);
      if (maze->pieces[i][j] == NULL) return NULL; //memory error
    }
  }
  return maze;
}

static square_t *mazePiece_new(int x, int y, int width, int height)
{
  square_t *square = malloc(sizeof(square_t));
  if (square == NULL) return NULL; //memory error

  square->visited = false;
  //west
  if (x == 0) square->west = true;
  else square->west = false;
  //east
  if (x == width-1) square->east = true;
  else square->east = false;
  //north
  if (y == 0) square->north = true;
  else square->north = false;
  //south
  if (y == height-1) square->south = true;
  else square->south = false;

  return square;
}



bool isValidMove(maze_t *maze, XYPos *moveFrom, XYPos *moveTo)
{
  uint32_t x1 = moveFrom->x; uint32_t x2 = moveTo->x;
  uint32_t y1 = moveFrom->y; uint32_t y2 = moveTo->y;


  if (x2+1 == 0 || y2+1 == 0) return false; //tried to move outside maze

  if (x1<x2) //move east
    return !maze->pieces[x1][y1]->east;
  else if (x1>x2) //move west
    return !maze->pieces[x1][y1]->west;
  else if (y1<y2) //move south
    return !maze->pieces[x1][y1]->south;
  else if (y1>y2) //move north
    return !maze->pieces[x1][y1]->north;
  else //why didn't you move??
    return false;
}

bool isDeadEnd(maze_t* maze, XYPos* lastPos, XYPos* currPos){
	uint32_t x_old = lastPos->x; uint32_t x_new = currPos->x;
	uint32_t y_old = lastPos->y; uint32_t y_new = currPos->y;

	if(x_old < x_new){ //moved east
		return (maze->pieces[x_old][y_old]->north &&
                maze->pieces[x_old][y_old]->west &&
                maze->pieces[x_old][y_old]->south &&
                !maze->pieces[x_old][y_old]->east);
	}
	else if (x_old > x_new){ //moved west
		return (maze->pieces[x_old][y_old]->north &&
                maze->pieces[x_old][y_old]->east &&
                maze->pieces[x_old][y_old]->south &&
                !maze->pieces[x_old][y_old]->west);
    }
	else if (y_old < y_new){ //moved south
		return (maze->pieces[x_old][y_old]->north &&
                maze->pieces[x_old][y_old]->west &&
                maze->pieces[x_old][y_old]->east &&
                !maze->pieces[x_old][y_old]->south);
    }
	else if (y_old > y_new){ //moved north
		return (maze->pieces[x_old][y_old]->east &&
                maze->pieces[x_old][y_old]->west &&
                maze->pieces[x_old][y_old]->south &&
                !maze->pieces[x_old][y_old]->north);
    }
	else return false; // Didn't move (should never happen)
}

void newDeadEndWall(maze_t *maze, XYPos *moveFrom, XYPos *moveTo)
{
  uint32_t x1 = moveFrom->x; uint32_t x2 = moveTo->x;
  uint32_t y1 = moveFrom->y; uint32_t y2 = moveTo->y;

  // if (isDeadEnd(maze,moveTo,moveFrom)) return; //don't box yourself in

  if (x1<x2){ //move east
    maze->pieces[x2][y2]->west = true; //dont move west
  }
  else if (x1>x2){ //move west
    maze->pieces[x2][y2]->east = true; //dont move east
  }
  else if (y1<y2){ //move south
    maze->pieces[x2][y2]->north = true; //dont move north
  }
  else if (y1>y2){ //move north
    maze->pieces[x2][y2]->south = true; //dont move south
  }
  else //where do you want the new wall??
    return;
}

void newWall(maze_t *maze, XYPos *moveFrom, XYPos *moveTo)
{
  uint32_t x1 = moveFrom->x; uint32_t x2 = moveTo->x;
  uint32_t y1 = moveFrom->y; uint32_t y2 = moveTo->y;

  if (x1<x2){ //move east
    maze->pieces[x1][y1]->east = true;
    maze->pieces[x2][y2]->west = true;
  }
  else if (x1>x2){ //move west
    maze->pieces[x1][y1]->west = true;
    maze->pieces[x2][y2]->east = true;
  }
  else if (y1<y2){ //move south
    maze->pieces[x1][y1]->south = true;
    maze->pieces[x2][y2]->north = true;
  }
  else if (y1>y2){ //move north
    maze->pieces[x1][y1]->north = true;
    maze->pieces[x2][y2]->south = true;
  }
  else //where do you want the new wall??
    return;
}

void maze_delete(maze_t *maze)
{
  for (int i = 0; i < maze->width; i++) {
    for (int j = 0; j < maze->height; j++)
      free(maze->pieces[i][j]);
    free(maze->pieces[i]);
  }
  free(maze->pieces);
  free(maze);
}

void printMaze(maze_t *maze)
{
  //print top row of walls
  for (int i = 0; i < maze->width; i++){
    printf(" ___");
  }
  printf("\n");
  //print the rest
  for (int j = 0; j < maze->height;j++) {
    for (int k = 0; k < 3; k++) {
      printf("|");
      for (int i = 0; i < maze->width; i++){
        if (k == 2 && maze->pieces[i][j]->south)
          printf("___");
        else if (k == 1 && beenVisited(maze,i,j))
          printf(" x ");
        else
          printf("   ");
        if (maze->pieces[i][j]->east)
          printf("|");
        else
          printf(" ");
      }
      printf("\n");
    }
  }
}


bool beenVisited(maze_t *maze, int x, int y)
{
  return maze->pieces[x][y]->visited;
}

void visit(maze_t *maze, XYPos *point)
{
  maze->pieces[point->x][point->y]->visited = true;
}

void writeToFile(maze_t *maze, FILE *fp)
{
    for (int i = 0; i < maze->height; i++){
        for (int j = 0; j < maze->width; j++){
            square_t *currPiece = maze->pieces[j][i];
            if(currPiece->north){
                fprintf(fp, "1 ");
            }
            else{
                fprintf(fp, "0 ");
            }
            if(currPiece->south){
                fprintf(fp, "1 ");
            }
            else{
                fprintf(fp, "0 ");
            }
            if(currPiece->east){
                fprintf(fp, "1 ");
            }
            else{
                fprintf(fp, "0 ");
            }
            if(currPiece->west){
                fprintf(fp, "1 ");
            }
            else{
                fprintf(fp, "0 ");
            }
        // fprintf(fp, "");
        }
        fprintf(fp, "\n");
    }
}
