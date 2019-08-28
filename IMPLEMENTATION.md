# Implementation

## Overview:
As described in the DESIGN.md explanation, the purpose of this program is to solve mazes of varying levels with different numbers of avatars. This document aims to explain the lower level processes happening in order for the program to "solve" the maze.

### Organization:
Client.c holds the main function for the program and is mostly responsible for initializing the connection between the server and the client. With the information received from the server, client.c initializes each avatar thread and passes each thread off to the avatar_starter function. The avatar_starter function holds the essence of the parsing and big picture functions that are required in the solving of the maze.

Many of the functions used are abstracted out into APIs. Our APIs include:
1. Avatar.[ch]
2. maze.[ch]
3. movements.[ch]
4. functions.[ch]

Additionally, there is a directory called `logFiles/` that holds the output files. Each file holds the record of the messages sent and received between the avatars and the servers.

Another file, `guiThing.txt` is a file that is written into during the program with the information being gathered for the maze. As the maze is updated, the file is also updated. This file is used by the `GUI.java` program in creating the visual interface of the maze and the avatars.

#### Structures:
Our program uses an avatar struct to hold various information needed throught the program. In particular, it holds a pointer to the maze struct. Each avatar will point to one maze struct, essentially sharing the maze like a global variable.
The maze struct holds a 2D array of mazePiece structs.
These hold key information in storing walls and directions.
```
typedef struct avatar avatar_t;
typedef struct maze maze_t;
typedef struct mazePiece square_t;
```
And others defined in common.h.

#### Client.c
This is the main program. We hold functions here critical to the startup of the program and the avatars.

```
avatar_t* create_avatar(int id, int *directions, int *nextMoves, XYPos **pos, int **paired, int MazePort, char *hostname, FILE *fp, maze_t *maze)
```
>This function is responsible for initializing each avatar struct.
Input: avatar id, pointer to the array of each avatars directions, pointer to array of nextMoves, a pointer to an array of pointers of XY structs and the paired, the mazeport number, the hostname, a file pointer to the log, and the pointer to the maze.
Output: an avatar struct holding all of the information given in the inputs.

```
char* createLogFile(int numAv, int diff, int MazePort)
```
> This function creates the logfile name and starts writing into it with the date, time, etc. 
Input: number of avatars, difficulty level, mazeport
Output: a pointer to the char array for the file name

```
struct Message* createMessage(int numAv, int diff)
```
>This function is responsible for creating the init message.
Input: number of avatars and the difficulty level
Output: the message.

```
struct Message* receiveMessage(int socket)
```
> This function receives the message from the server and parses it to make sure it is an init message
>Input: socket number
>Output: the received message

#### Avatar.h
The avatar API holds the meat of the avatar communication  throughout the maze solving process. Here, the avatars call the functions to read the server's message, decide their next move, and send it to the server.

```
void ready_message(Message *msg, int id);
```
> This function is responsible for crafting the ready message to send to the server upon initialization of the avatar thread.
Input: a pointer to a message, avatar ID
Output: none. This function will write into the message that was passed into the function.
```
Message* choose_next_move(Message *recvMsg, avatar_t *avatar);
```
> This function is responsible for deciding what next move to take. Based on their updated maze data, the avatar in turn will choose a cardinal direction to
move and will put all the info in a message.
Input: the message received from server, the array of threads
Output: message with the move and the id of moving avatar (to be sent in client.c). Failure to allocate space for the Message will result in an error message being printed.

```
void maze_solving(int socket, avatar_t *avatar);
```
> This function is the center of this project. It will receive a message from the server and decide what the program should do based on the message. If it is an avatar_turn message, the function will call another function to decide the next move.
Input: The socket value, the avatar struct
Output: nothing. On exit from the function, we return to the avatar_starter function.

```
void* avatar_starter(void* av);
```
> This function is responsible for starting the connections for each thread and sending the ready message to the server. It is called by the client.c pthread_create function with a void* pointer to an avatar struct.
There are no outputs. On exit of this function, we return to the main client.c program to clean up.



#### Maze.h
This API holds the functions to initialize the maze structure, add new walls, check for dead ends, and so on.

```
maze_t * maze_new(int width, int height);
```
> Creates a maze object for the given width and height.
The maze consists of mazePiece objects with wall values,
one for each square in the maze. The values correspond to
whether or not a wall has been found there.
Input: width and height of maze
Output: a maze_t struct if succesful. NULL if malloc fails to allocate space

```
bool isValidMove(maze_t *maze, XYPos *moveFrom, XYPos *moveTo);
```
>Checks to see if the working maze already has a wall
where player wants to move. Returns, from past
experience, whether the avatar will hit a wall.
We assume the move is exactly one step in a cardinal direction.
Input: a maze_t struct, current position of avatar in XYPos struct, and the position the avatar is attempting to move to
Output: true the move is valid, false if there is a wall there or if it is at the outer walls of the maze

```
bool isDeadEnd(maze_t* maze, XYPos *lastPos, XYPos *currPos);
```
>Checks to see if the previous location of the avatar is a dead end so that a wall can be created. Returns true is there are walls in the three other directions different from the avatar's movement.
Input: maze_t struct, the previous position of the avatar, and the position it is at right now after moving
Output: true if it is a dead end, false if it is not

```
void newWall(maze_t *maze, XYPos *moveFrom, XYPos *moveTo);
```
>Creates a new wall if the server says a move is invalid.
Again, we assume the move is exactly one step in a cardinal direction.
Input: maze_t struct, location avatar is at, and the location is tried to move to
Output: no output, but will mark the wall in the maze_t struct pieces if it was a wall otherwise, it does not do anything

```
void newDeadEndWall(maze_t *maze, XYPos *moveFrom, XYPos *moveTo);
```
>Creates a "dead end wall", which prevents avatars from entering a
dead end without trapping avatars already in the dead end.
Input: a maze_t struct, current position of avatar, position avatar attempted to move to
Output: no output, it will mark a wall to block off the dead-end in the maze_t struct otherwise, it will do nothing

```
void printMaze(maze_t *maze);
```
>Prints out the maze to stdout.
This is mostly for debugging purposes.
Does not print out the location of each avatar.
Input: maze_t struct
Output: prints the maze at the point in the program

```
void maze_delete(maze_t *maze);
```
>Deletes the maze_t struct before program quits. Clean-up
Input: maze_t struct
Output: nothing, just the freeing of space allocated for the maze_t struct

```
bool beenVisited(maze_t *maze, int x, int y);
```
>Returns whether a square on the board has been visited
Input: maze_t struct, x and y position that is being checked
Output: true if visited, false if not

```
void writeToFile(maze_t *maze, FILE *fp);
```
>Prints the contents of the maze to a file for the GUI to read, prints in 0's and 1's
Input: maze_t struct, a file pointer to the maze log
Output: prints to the file to log


#### Movement.h
This is an API for different movement heuristics we have come up with. Each avatar passes in the requesite information and the movement function determines what the avatar's next move will be, and returns the cardinal direction to go next. This API holds much of the integral logic of the maze solving algorithm.
```
int move_random(maze_t *maze, XYPos *current);
```
>This function generates a random direction to move, sees if said move is valid, and returns that direction if it is.
A valid move is defined by: the maze struct does not have a wall where the avatar is trying to move. Either no wall exists there or it has not yet been explored.
NOTE: This function was used in the early stages of development. The final program does not use this function.

```
int move_wallFollower(maze_t *maze, XYPos *current, int inputDirection);
```
>This function holds the key logic of the maze solving algorithm. Essentially,
the avatar is following the maze, blind, by placing its "right hand" on the wall
and following that wall until the maze is "solved".
The previous direction is important in deciding what direction
to turn with the wall. In short, if the wall has a turn to the right, the avatar will
turn right with it. Otherwise, the avatar will progressively turn left until a valid move.
Input: maze_t struct, current XY position of the avatar, and the direction the avatar moved last time.
Output: the new direction avatar should go, in integer representation of the cardinal movements defined in common.h

#### Functions.h
This API holds functions that are can be abstracted out of the official client.c program. Some of these functions are shared amongst the client.c and avatar.c programs.
```
struct hostent* extractIP(char *host);
```
>This function is reponsible for extracting the IP information from the host. The output is in a hostent* struct form. This is used in the initialization of connections in client.c and avatar.c
Input: a char pointer to the host name given by the original user input. This program specifically runs on moose.cs.dartmouth.edu host server.
Output: if succesful, the function will returan a hostent* struct that holds the IP information. If the allocation of space was unsuccesful, the program will quit with an exit code 5, with an error statement printed to stdout.

```
void delete_avatar(avatar_t **list, int numAv);
```
>This function is used in the clean up of the program as it is finishing. It will free the space originally allocated for the array of avatar structs initialized in client.c
Input: a pointer to an array of pointers all pointing to avatar structs and the number of avatars
Output: no output. Upon failure to free the space, there will be an invalid free or memory leakage, shown when run with valgrind

```
void delete_func(XYPos **list, int numAv);
```
>This function is used in cleaning up the space allocated for the array of avatar threads. It functions essentially the same as delete_avatar, but is specified for a different input type.
Input: a pointer to an array of pointers, all pointing to an initialized avatar pthread, and the number of avatars.
Output: no output. Upon failure to free the space, there will be an invalid free or memory leakage, shown when run with valgrind
