/*
 * Anon 19X: Kyle Bensink, Ashley Song, Rylee Stone, Barett Noone
 *
 * functions.h - an API for functions that are can be abstracted out of the official
 * client.c program. Some of these functions are shared amongst the client.c and avatar.c
 * programs.
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
#include "common.h"
#include "./avatar.h"

/****************Functions***************************/

/******************** extractIP() ********************/
/*
This function is reponsible for extracting the IP information from
the host. The output is in a hostent* struct form. This is used in the
initialization of connections in client.c and avatar.c

Input: a char pointer to the host name given by the original user input.
      This program specifically runs on moose.cs.dartmouth.edu host server.
Output: if succesful, the function will returan a hostent* struct that
        holds the IP information. If the allocation of space was unsuccesful,
        the program will quit with an exit code 5, with an error statement printed
        to stdout.
*/
struct hostent* extractIP(char *host);

/******************** delete_avatar() ********************/
/*
This function is used in the clean up of the program as it is finishing.
It will free the space originally allocated for the array of avatar structs initialized
in client.c

Input: a pointer to an array of pointers all pointing to avatar structs and
      the number of avatars
Output: no output. Upon failure to free the space, there will be an invalid free or
        memory leakage, shown when run with valgrind
*/
void delete_avatar(avatar_t **list, int numAv);

/******************** delete_func()********************/
/*
This function is used in cleaning up the space allocated for the array of
avatar threads. It functions essentially the same as delete_avatar, but is
specified for a different input type.

Input: a pointer to an array of pointers, all pointing to an initialized
      avatar pthread, and the number of avatars.
Output: no output. Upon failure to free the space, there will be an invalid free or
        memory leakage, shown when run with valgrind
*/
void delete_func(XYPos **list, int numAv);
