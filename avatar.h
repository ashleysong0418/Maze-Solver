/*
 * Anon 19X: Kyle Bensink, Ashley Song, Rylee Stone, Barett Noone
 *
 * avatar.h holds the meat of the avatar communication
 * throughout the maze solving process. Here, the avatars
 * call the functions to read the server's message, decide
 * their next move, and send it to the server.
 *
*/
/***************System Includes******************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
// #include <netdb.h>
#include <unistd.h>
// #include <sys/socket.h>
// #include <pthread.h>
// #include <arpa/inet.h>
// #include <netinet/in.h>
// #include <sys/types.h>
// #include <sys/socket.h>

/*****************Local Files Included********************/
#include "./libcs50/memory.h"
#include "./maze.h"
#include "./movement.h"
#include "common.h"


/**************Structures***************/
typedef struct avatar avatar_t;

/****************Functions***************************/

/******************** avatar_starter() ********************/
/*
This function is responsible for starting the connections for each thread and sending the ready message to the server. It is called by the client.c pthread_create function with a void* pointer to an avatar struct.
There are no outputs. On exit of this function, we return to the main client.c program to clean up.
*/
void* avatar_starter(void* av);


/******************** ready_message() ********************/
/*
This function is responsible for crafting the ready message to send to the server upon initialization of the avatar thread.
Input: a pointer to a message, avatar ID
Output: none. This function will write into the message that was passed into the function.
*/
void ready_message(Message *msg, int id);

/******************** maze_solving() ********************/
/*
This function is the center of this project. It will receive a message from the server and decide what the program should do based on the message. If it is an avatar_turn message, the function will call another function to decide the next move.
Input: The socket value, the avatar struct
Output: nothing. On exit from the function, we return to the avatar_starter function.
*/
void maze_solving(int socket, avatar_t *avatar);

/******************** choose_next_move() ********************/
/*
This function is responsible for crafting the ready message to send to the server upon initialization of the avatar thread.
Input: a pointer to a message, avatar ID
Output: none. This function will write into the message that was passed into the function.
*/
Message* choose_next_move(Message *recvMsg, avatar_t *avatar);
/*
This function is responsible for checking if the avatar moved succesfully or not
Input: avatar struct array, received message from server (AVATAR_TURN), number of avatars
Output: a number that refers to a cardinal direction
*/
// static int didMove(XYPos *currPos, XYPos *newPos);

/*
This function is responsible for deciding what next move to take.
Based on their updated maze data, the avatar in turn will choose a cardinal direction to
move and will put all the info in a message.
Input: the message received from server, the array of threads
Output: message with the move and the id of moving avatar (to be sent in client.c)
*/

Message* choose_next_move(Message *recvMsg, avatar_t *avatar);
