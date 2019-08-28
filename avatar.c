/*
* Team: ANON
* Members: Ashley Song, Barett Noone, Kyle Bensink, Rylee Stone
* CS50, 19X, The Maze Challenge
*
* avatar.c holds the functions to start run the constant communication between
* the server and the avatars.
*
*/

/***************System Includes******************/
#include "avatar.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>
#include <netdb.h>
#include <unistd.h>
#include <sys/socket.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>

/*****************Local Files Included********************/
#include "./libcs50/memory.h"
#include "./avatar.h"
#include "./maze.h"
#include "./movement.h"
#include "common.h"
#include "functions.h"
/**************Structures***************/
typedef struct avatar{
  int id;
  int *direction; //[MAX_AVATAR];
  int *nextMoves;
  maze_t *maze;
  XYPos **pos; //[MAX_AVATAR];
  int MazePort;
  char *hostname;
  FILE *fp;
  // char[MAX_MESSAGE];
  int nAvatars;
  int **paired;
} avatar_t;

/************Static Functions*****************/
int didMove(XYPos *currPos, XYPos *newPos);
static int check_message(Message *msg, int msgCheck, FILE *fp, int numAv);
void addWall(maze_t *maze, XYPos *currPos, XYPos *newPos, int dir);
void logMaze(maze_t* maze, int numAvs, XYPos** pos, char *fileName);
/****************Global***********************/
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER; //for checking Messages
pthread_mutex_t lock_move = PTHREAD_MUTEX_INITIALIZER;  //for choose next move
/****************Functions***************************/

void* avatar_starter(void* av){
  avatar_t *avatar = (avatar_t*)av;
  struct hostent *IPhost = extractIP(avatar->hostname);    //free later
  struct sockaddr_in server;
  server.sin_family = AF_INET;
  server.sin_port = htons(avatar->MazePort);
  memcpy(&server.sin_addr, IPhost->h_addr_list[0], IPhost->h_length);

  //create a socket
  int theSock = socket(AF_INET, SOCK_STREAM, 0);
  if(theSock < 0){
    fprintf(stderr, "Error: opening socket\n");
    exit(6);
  }
  //connect the socket
  if(connect(theSock, (struct sockaddr *) &server, sizeof(server)) < 0){
    fprintf(stderr, "Error: connecting socket\n");
    exit(7);
  }

  printf("Avatar Connected!\n");

  //create and send a ready message to server
  Message *readyMsg = assertp(count_calloc(1, MAX_MESSAGE + 1), "Message initialization failed");
  ready_message(readyMsg, avatar->id);
  send(theSock, readyMsg, sizeof(Message), 0);
  count_free(readyMsg);
  sleep(1); //sleep for a second

  maze_solving(theSock, avatar);  //start maze solving process

  return NULL;  //return to client.c main function to clean up
}

void ready_message(Message *msg, int id){
  msg->type = htonl(AVATAR_READY);
  msg->avatar_ready.AvatarId = htonl(id);
}

void maze_solving(int socket, avatar_t *avatar){
  Message *recvMsg = assertp(count_calloc(1, MAX_MESSAGE + 1), "Failed to retrieve message from server");
  int msgCheck; //for checking that message was received correctly
  while(true){
    msgCheck = recv(socket, recvMsg, sizeof(Message), 0);
    if (recvMsg == NULL) {
        // pthread_mutex_unlock(&lock);
        continue; //don't check the contents of the message
    }
    // printf("got a message!");
    int status = check_message(recvMsg, msgCheck, avatar->fp, avatar->nAvatars);
    if(status == 1){  //if message type indicates fatal error, quit solving the maze and abort the program
      count_free(recvMsg);
      return;
    }
    else if(status == 2){ //if 2, maze was solved, exit and go clear everything
      //update positions for printin;
      fprintf(avatar->fp, "MAZE SOLVED! nAvatars:%u difficulty:%u nMoves:%u Hash:%u\n",
        (unsigned)ntohl(recvMsg->maze_solved.nAvatars),
        (unsigned)ntohl(recvMsg->maze_solved.Difficulty),
        (unsigned)ntohl(recvMsg->maze_solved.nMoves),
        (unsigned)ntohl(recvMsg->maze_solved.Hash));  //print into file that maze was solved, include hash value
      logMaze(avatar->maze, avatar->nAvatars, avatar->pos, "./guiThing.txt"); //put into maze file the last updates
      count_free(recvMsg);
      return;
    }
    else if(status == 0){
      unsigned int castedMessage = (unsigned)ntohl(recvMsg->avatar_turn.TurnId);  //extract avatar turn ID
      if(castedMessage == avatar->id){  //if it is this avatar's turn, get next move and send as message to server
        // printf("Av:%d Line: 110\n", avatar->id);
        Message *sendMsg = choose_next_move(recvMsg, avatar);
        fprintf(avatar->fp, "Send: AVATAR_MOVE:%u Direction:%u\n", castedMessage, (unsigned)ntohl(sendMsg->avatar_move.Direction));
        send(socket, sendMsg, sizeof(Message), 0);  //send the next move message to server
        count_free(sendMsg);
      }
      sleep(0.5);
    }
    else{
       // printf("%d else\n", avatar->id);
       count_free(recvMsg);
       return;
   }
  }
  count_free(recvMsg);
}

static int check_message(Message *msg, int msgCheck, FILE *fp, int numAv){
  //if 1 is returned, abort/finish program
  //if 2 returned, just print to log, continue program
  //if 0 returned, move on to parse the next avatar move messages from server
  // pthread_mutex_lock(&lock);
  if(msgCheck == 0){
    fprintf(fp, "Error: Unable to receive message from server\n");
  }
  if(ntohl(msg->type) == AVATAR_TURN){
    fprintf(fp, "Received: AVATAR_TURN:%u ", (unsigned)ntohl(msg->avatar_turn.TurnId));  //not %s, what else?
    for(int i = 0; i < numAv; i++){
      fprintf(fp, "AV%d:(%u,%u) ", i, (unsigned)ntohl(msg->avatar_turn.Pos[i].x), (unsigned)ntohl(msg->avatar_turn.Pos[i].y));
    }
    fprintf(fp, "\n");
    return 0;
  }
  else if (ntohl(msg->type) == MAZE_SOLVED){
    fprintf(fp, "MAZE SOLVED!\n");
    fprintf(stdout, "MAZE SOLVED!\n");
    return 2;
  }
   else if (IS_ERROR(ntohl(msg->type))){
    if(ntohl(msg->type) == INIT_FAILED){
      fprintf(fp, "Error: Initialization Failed\n");
    }
    else if(ntohl(msg->type) == UNEXPECTED_MSG_TYPE){
      fprintf(fp, "Error: Unexpected Message Type\n");
    }
    else if(ntohl(msg->type) == UNKNOWN_MSG_TYPE){
      fprintf(fp, "Error: Unknown Message Type\n");
    }
    else{
      fprintf(fp, "Error: Unknown error of some type, initialization failed\n");
    }
    if(ntohl(msg->type) == SERVER_TIMEOUT){
      fprintf(fp, "SERVER TIMEOUT\n");
    }
    else if(ntohl(msg->type) == TOO_MANY_MOVES){
      fprintf(fp, "TOO MANY MOVES\n");
    }
    else if(ntohl(msg->type) == SERVER_OUT_OF_MEM){
      fprintf(fp, "SERVER OUT OF MEMORY\n");
    }
    else if(ntohl(msg->type) == UNKNOWN_MSG_TYPE){
      fprintf(fp, "UNKNOWN_MSG_TYPE\n");
    }
    else if(ntohl(msg->type) == NO_SUCH_AVATAR){
      fprintf(fp, "NO_SUCH_AVATAR\n");
    }
    else if(ntohl(msg->type) == AVATAR_OUT_OF_TURN){
      fprintf(fp, "AVATAR_OUT_OF_TURN\n");
    }
    return 1;
  }

  // printf("Line: 181\n");
  return 1;
}

Message* choose_next_move(Message *recvMsg, avatar_t *avatar){
  //one avatar chooses next move, creates the message, sends to server
  //parse the character to finalize the cardinal direction
  // pthread_mutex_lock(&lock_move);
  Message *message = assertp(count_calloc(1, MAX_MESSAGE + 1), "Failed to retrieve message from server");
  uint32_t avTurnId = ntohl(recvMsg->avatar_turn.TurnId); //next avatar to move

  //avatar with id = avTurnId will decide which way to move
  XYPos *currPos = avatar->pos[avTurnId];
  int moved = -1;
  XYPos *newPos = assertp(count_calloc(1, sizeof(XYPos)), "Initialization of newPos failed\n");
  if (currPos == NULL) { // first turn
      newPos->x = ntohl(recvMsg->avatar_turn.Pos[avTurnId].x);
      newPos->y = ntohl(recvMsg->avatar_turn.Pos[avTurnId].y);
      avatar->pos[avTurnId] = newPos;
      currPos = avatar->pos[avTurnId];
      avatar->direction[avTurnId] = M_NORTH;
  } else {
      newPos->x = ntohl(recvMsg->avatar_turn.Pos[avTurnId].x);
      newPos->y = ntohl(recvMsg->avatar_turn.Pos[avTurnId].y);
      moved = didMove(currPos, newPos); //compare the two positions to see if move was succesful
      if(moved != M_NULL_MOVE){ //if it didn't move
          avatar->direction[avTurnId] = avatar->nextMoves[avTurnId];
          if(isDeadEnd(avatar->maze, currPos, newPos)){ //if it was dead end, block off the area
            newDeadEndWall(avatar->maze, currPos, newPos);
          }
      }
      else{ //if it didn't move, add a wall in the maze
          // printf("didn't move\n");
          addWall(avatar->maze, currPos, newPos, avatar->nextMoves[avTurnId]);  //update the maze with the new info from received Message
      }
      avatar->pos[avTurnId]->x = ntohl(recvMsg->avatar_turn.Pos[avTurnId].x);
      avatar->pos[avTurnId]->y = ntohl(recvMsg->avatar_turn.Pos[avTurnId].y);
      count_free(newPos);
  }
  for (int i = 0; i < avatar->nAvatars; i++) {  //check if any of the avatars have met up.
      if (i == avTurnId) continue;
      if ((unsigned)ntohl(recvMsg->avatar_turn.Pos[i].x) == avatar->pos[avTurnId]->x &&
          (unsigned)ntohl(recvMsg->avatar_turn.Pos[i].y) == avatar->pos[avTurnId]->y) {
            if (!avatar->paired[i][avTurnId]){
              printf("Two avatars collided!\n");
              avatar->direction[avTurnId] = avatar->nextMoves[i];
              avatar->nextMoves[avTurnId] = avatar->nextMoves[i];
              avatar->paired[avTurnId][i] = 1;  //if met up, pair them so they move together
              avatar->paired[i][avTurnId] = 1;
          }
      }
  }
  logMaze(avatar->maze,avatar->nAvatars,avatar->pos,"./guiThing.txt");  //log the maze for the GUI
  // printf("passed in: %d\n", avatar->direction[avTurnId]);
  int nextMove = move_wallFollower(avatar->maze, currPos, avatar->direction[avTurnId]); //get next move ß
  avatar->nextMoves[avTurnId] = nextMove;
  // printf("result: %d\n",nextMove);
  printMaze(avatar->maze);

  //then it will construct the message to send to server
  message->type = htonl(AVATAR_MOVE);
  message->avatar_move.AvatarId = htonl(avTurnId);  //which avatar is moving
  message->avatar_move.Direction = htonl(nextMove);

  return message;
}

int didMove(XYPos *currPos, XYPos *newPos){
  if(currPos->x < newPos->x){
    return M_EAST;
  }
  else if (currPos->x > newPos->x){
    return M_WEST;
  }
  else if (currPos->y < newPos->y){
    return M_SOUTH;
  }
  else if (currPos->y > newPos->y){
    return M_NORTH;
  }

  return M_NULL_MOVE;
}


void addWall(maze_t *maze, XYPos *currPos, XYPos *newPos, int dir)
{
    switch(dir) {
        case M_EAST:
            newPos->x++; break;
        case M_WEST:
            newPos->x--; break;
        case M_SOUTH:
            newPos->y++; break;
        case M_NORTH:
            newPos->y--; break;
        default: printf("dir = %d\n",dir); return;
    }
    newWall(maze,currPos,newPos);
}

void logMaze(maze_t *maze, int numAvs, XYPos **pos, char *fileName){
    char *lockCMD = assertp(count_calloc(100,sizeof(char)),"Can't make a string");
    sprintf(lockCMD,"chmod a-r %s",fileName);
    char *unlockCMD = assertp(count_calloc(100,sizeof(char)),"Can't make a string");
    sprintf(unlockCMD,"chmod a+r %s", fileName);

    bool flag =true;
    FILE *fp;
    while (flag) {
        fp = fopen(fileName,"w");
        if (fp == NULL)
            sleep(0.1); //wait for the java file to finish reading from the file
        else
            flag = false;
    }

    system(lockCMD);  //bash commands to change permissions on file for change in use between GUI and this program
	/* Log avatars and their positions */
	for(int i = 0; i < numAvs; i++){
        if (pos[i] != NULL)
		      fprintf(fp, "%u %u\n", pos[i]->x, pos[i]->y);
	}

	// printf(fp, "% %u\n", maze->width, maze->height);

	/* Log the current structure of the maze */
	writeToFile(maze,fp);


    fclose(fp);
    system(unlockCMD);
    count_free(lockCMD);
    count_free(unlockCMD);
}
