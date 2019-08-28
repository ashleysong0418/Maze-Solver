/*
* Team: ANON
* Members: Ashley Song, Barett Noone, Kyle Bensink, Rylee Stone
* CS50, 19X, The Maze Challenge
*
* functions.h API holds some of the shared functions among client.c and avatar.c
*
*
*
*/

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
#include "./libcs50/memory.h"
// #include "./maze.h"
// #include "./movement.h"
#include "common.h"
#include "./avatar.h"

struct hostent* extractIP(char *host){
  struct hostent *IPaddress;
  IPaddress = gethostbyname(host);
  if(IPaddress == NULL){
    fprintf(stderr, "Unknown host '%s'\n", host);
    exit(5);
  }
  return IPaddress;
}

void delete_avatar(avatar_t **list, int numAv){

  for(int i = 0; i < numAv; i++){
    count_free(list[i]);
  }
  count_free(list);
}

void delete_func(XYPos **list, int numAv){

  for(int i = 0; i < numAv; i++){
    count_free(list[i]);
  }
  count_free(list);
}
