/*
* Team: ANON
* Members: Ashley Song, Barett Noone, Kyle Bensink, Rylee Stone
* CS50, 19X, The Maze Challenge
*
* Client.c holds the program to start the client connection with the server.
*
*///mygcc -o client client.c -lpthread


/***************System Includes******************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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
#include "common.h"
#include "libcs50/memory.h"
#include "avatar.h"
#include "maze.h"
#include "movement.h"
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

/**************Functions*****************/
avatar_t* create_avatar(int id, int *directions, int *nextMoves, XYPos **pos, int **paired, int MazePort, char *hostname, FILE *fp, maze_t *maze);
char* createLogFile(int numAv, int diff, int MazePort);
struct Message* createMessage(int numAv, int diff);
struct Message* receiveMessage(int socket);
/************Static Functions*****************/



/*************Main Code******************/
int main(int argc, char *argv[]) {
  //check number of arguments: should be 4
  if(argc != 4){
    fprintf(stderr, "Usage: ./client <nAvatars> <difficulty (0-9)> <hostname>\n");
    exit(1);
  }

  int nAvatars = atoi(argv[1]);
  int difficulty = atoi(argv[2]);
  char *hostname = argv[3];
  //check num of avatars is less than max
  if(nAvatars > MAX_AVATAR){
    fprintf(stderr, "Number of avatars exceeded MAX; Max is %d\n", MAX_AVATAR);
    exit(2);
  }

  //check difficulty level is [0,9]
  if((difficulty < 0) && (difficulty> 9)){
    fprintf(stderr, "Difficulty level out of range; intput number [0,%d]\n", MAX_DIFFICULTY);
    exit(3);
  }

  //check valid hostname??????
  if(strcmp(argv[3], "moose.cs.dartmouth.edu") != 0){
    fprintf(stderr, "Invalid hostname\n");
    exit(4);
  }


  //get the IP of the host
  struct hostent *IPhost = extractIP(hostname);    //free later
  struct sockaddr_in server;
  server.sin_family = AF_INET;
  server.sin_port = htons(SERVER_PORT);
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
  printf("Socket Connected!");


  //create the message for the server
  Message *init_message = createMessage(nAvatars, difficulty);

  //send the message
  send(theSock, init_message, sizeof(Message), 0);
  free(init_message); //do not need anymore after sending

  //recieve message from server
  Message *received = receiveMessage(theSock);
  if(htonl(received->type) != INIT_OK){
    fprintf(stderr, "INIT_OK message not received\n");
  }
  int MazePort = ntohl(received->init_ok.MazePort);
  int MazeHeight = ntohl(received->init_ok.MazeHeight);
  int MazeWidth = ntohl(received->init_ok.MazeWidth);
  char *logFile = createLogFile(nAvatars, difficulty, MazePort);
  count_free(received);

  if (fork()) {
      sleep(1);
      //initialize the nAvatars???????????????
      FILE *fp = fopen(logFile, "a"); //append to file, not write over it
      char *IPaddy = inet_ntoa(*((struct in_addr*)IPhost->h_addr_list[0]));

      close(theSock);
    /**************************Avatar Threads******************************/
      //the array of avatar threads
      pthread_t *thread_id = assertp(count_calloc(nAvatars, sizeof(pthread_t)), "pthread_t initialization failed\n");
      avatar_t **avatar_list = assertp(count_calloc(nAvatars, sizeof(avatar_t*)), "avatar struct list initialization failed\n");
      maze_t *maze = maze_new(MazeWidth, MazeHeight);
      int *directions = assertp(count_calloc(nAvatars, sizeof(int)), "Directions array failed to initialize\n");
      int *nextMoves = assertp(count_calloc(nAvatars, sizeof(int)), "NextMoves array failed to initialize\n");
      XYPos **pos = assertp(count_calloc(nAvatars, sizeof(XYPos*)), "Positions array failed to initialize\n");
      int **paired = assertp(count_calloc(nAvatars,sizeof(int*)), "Paired array failed to initialize\n");
      for (int i = 0; i < nAvatars; i++){
        paired[i] = assertp(count_calloc(nAvatars,sizeof(int)), "Paired array failed to initialize\n");
        for (int j = 0; j < nAvatars; j++)
            paired[i][j] = 0;
      }
      /* for testing */
      // char s1[] = "abcdefg";
      // char s2[] = "xyz";

      for(int i = 0; i < nAvatars; i++){
        fprintf(fp, "%s %d %d %d %s %d %s\n", argv[0], i, nAvatars, difficulty, IPaddy, MazePort, logFile);
        avatar_list[i] = create_avatar(i, directions, nextMoves, pos, paired, MazePort, hostname, fp, maze);
        avatar_list[i]->nAvatars = nAvatars;
        int iret_id =  pthread_create(&thread_id[i], NULL, avatar_starter, avatar_list[i]);
        if (iret_id != 0) {
          fprintf(stderr,"Cannot create thread, rc=%d\n", iret_id);
        }
        else{
          printf("Connected %d\n", i);
        }
      }


      //create new socket for the threads
      for(int j = nAvatars-1; j >= 0; j--){
        int iret_join = pthread_join(thread_id[j], NULL);
        if(iret_join != 0){
          fprintf(stderr,"Cannot join thread: %d\n", iret_join);
        }
        else{
          printf("Joined: %d\n", j);
        }
      }

      fclose(fp);
      printf("FINISH\n");
      for (int i = 0; i < nAvatars; i++) {
          count_free(pos[i]);
      }
      count_free(pos);
      count_free(directions);
      count_free(nextMoves);
      for (int i = 0; i < nAvatars; i++) {
          count_free(paired[i]);
      }
      count_free(paired);
      delete_avatar(avatar_list, nAvatars);
      maze_delete(maze);
      count_free(thread_id);
      count_free(logFile);
      // sleep(5);
      exit(0);

  }
  else {
      char *guiStart = assertp(count_calloc(100,sizeof(char)),"Can't make a string");
      sprintf(guiStart,"java GUI %d %d %d guiThing.txt",MazeWidth,MazeHeight,nAvatars);
      system(guiStart);
      count_free(guiStart);
      count_free(logFile);
      // printf("done with the GUI");
      exit(0);
  }
}


avatar_t* create_avatar(int id, int *directions, int *nextMoves, XYPos **pos, int **paired, int MazePort, char *hostname, FILE *fp, maze_t *maze){
  //initialize avatar with its id and initial directions, etc
  avatar_t *newAv = assertp(count_calloc(1, sizeof(avatar_t)), "Failed to create new avatar_t\n");
  newAv->id = id;
  newAv->direction = directions;
  newAv->nextMoves = nextMoves;
  newAv->pos = pos;
  newAv->paired = paired;
  newAv->MazePort = MazePort;
  newAv->hostname = hostname;
  newAv->fp = fp; //open file to log file
  newAv->maze = maze;

  return newAv;
}

char* createLogFile(int numAv, int diff, int MazePort){
  //maze_$USER_N_D.log,
  char *user = getenv("USER");
  printf("%s", user);
  char *logFile = assertp(count_calloc(strlen("./logFiles/maze_") + 75, sizeof(char)), "Log File creation failed\n");
  FILE *fp;
  sprintf(logFile, "./logFiles/maze_%s_%d_%d.log", user, numAv, diff);
  printf("%s\n", logFile);
  fp = fopen(logFile, "w");
  if(fp != NULL){
    time_t timeDate;  //get the current time and date
    time(&timeDate);
    fprintf(fp, "%s %d %s\n", user, MazePort, ctime(&timeDate));    //print into file
  }
  fclose(fp);
  return logFile;
}

struct Message* createMessage(int numAv, int diff){
  Message *message = assertp(count_calloc(1, MAX_MESSAGE + 1), "Message initialization failed");

  message->type = htonl(INIT);
  message->init.nAvatars = htonl(numAv);
  message->init.Difficulty = htonl(diff);

  return message;
}

struct Message* receiveMessage(int socket){
  Message *message = assertp(count_calloc(1, MAX_MESSAGE + 1), "Failed to retrieve message from server");
  int length = recv(socket, message, sizeof(Message), 0);
  if(length == 0){
    fprintf(stderr, "Error: Unable to receive message from server\n");
    free(message);
    exit(8);
  }
  //check Message
  if(IS_ERROR(ntohl(message->type))){
    if(ntohl(message->type) == INIT_FAILED){
      fprintf(stderr, "Error: initialization failed\n");
      free(message);
      exit(9);
    }
    else if(ntohl(message->type == UNKNOWN_MSG_TYPE)){
      fprintf(stderr, "Error: Unknown message type\n");
      free(message);
      exit(10);
    }
    else{
      fprintf(stderr, "Error: unknown error of some type, initialization failed\n");
      free(message);
      exit(11);
    }
  }
  return message;
}
