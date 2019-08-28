# Design Specs
## Group Anon, CS50 19X
## Rylee Stone, Ashley Song, Kyle Bensink, Barrett Noone


### General Information:
The purpose of this program is to solve mazes of varying levels. Each increasing level presents a larger, more complex maze in which a varying number of avatars must explore blindly. The maze is "solved" when all avatars have converged to a common position, wherever that may be.

Each avatar is "blind" to the maze, as it does not know where the walls and dead ends are. Their knowledge of the maze structure is stored as they explore. However, avatars can "shout" to each other. In lower level terms, this means that the server sends the XY position of each avatar after every turn, in which each avatar can access the coordinates of the other avatars.

Maze levels vary from 0-9, and the number of avatars is anywhere from 1-10 avatars.


### Definitions:
1. Avatars: each member of the game, threads connected to the server
2. Difficulty: determines how complex and large the maze is (maze is created by the server)
3. Host: moose.cs.dartmouth.edu
4. LogFile: the directory/file that will hold the info that is tracking the avatar movements, messages sent and recieved
5. guiThing.txt: the file we designated as the place where the contents of the maze are logged for the purpose of the GUI program parsing it for the user interface

### Compiling and Running:
> Download XQuartz terminal
> Open XQuartz, go to "Applications" and select "terminal"
> log into server with ssh -Y user@moose.cs.dartmouth.edu
> use `make` to compile client.c program
> use `make withGui` to compile Java GUI
> Run with: `./client <number of avatars> <difficulty> moose.cs.dartmouth.edu`
> use `make clean` to clean up all compiled files

### Input:
- To `client` - nAvatars, Difficulty, mazeServerName
- To each `avatar` - `client` passes avatarID, nAvatars, Difficulty, serverIPAddress, MazePort, logFileName


### Output:
- `client` is just a script and has no output
- `avatar` has no output to user besides interacting with server
    * sends `AVATAR_TURN` messages


### Data Flow:
1. `client` establishes port connection to server, initializes each `avatar`
2. For each `avatar` move until `MAZE_SOLVED`:
    3. process `AVATAR_TURN` message from server
    4. Determine whether previous move was successful
    5. Update maze
    5. Make movement decision (follow the right wall, if two avatars meet, they will align their directions and move together).
    6. send `AVATAR_MOVE` message to server
7. Move until maze is solved, time is out, max movement reached, server time out, or until possible error encountered.


### API Definitions:
- `avatar.c`/`avatar.h`: contains functions and logic for the avatars' behaviors and communications with the server.
- `maze.c`/`maze.h`: provides structs and related functions for storing the structure of the maze as it is explored by the avatars.
- `gui.c`/`gui.h`: planned to contain functions to process graphic representations of maze and avatar movement.


### Major Data Structures:
1. `Maze`, a 2D array of `mazePieces` (not visible to other files) which track the location of discovered walls
2. `avatar`: contains the avatar's ID number, the direction it is facing, the ID for its next move, the maze data structure, the (x,y) positions for each avatar, the MazePort number, the hostname, a file pointer for logging, the number of avatars and a double array that is updated if avatars get 'paired' together (if they collide and are now moving the same direciton).


### Client-Server Initial Connection Pseudocode (client):
> usage: `./client <nAvatars> <difficulty> <moose.cs.dartmouth.edu>`
1. validate the arguments.
2. Extract the IP
3. Create a socket
4. Connect the socket
5. If socket connection succesful:
    - create message for server with nAvatars, difficulty
    - send the message
    - receive message from server
        - If "INIT_OK" message received:
            - gather MazePort info
    - create logFile name
    -
    - initialize avatar threads
        - join the threads
    - log into the file for first line: $USER, the MazePort, and the date & time
    - log into the file for each succesful avatar thread: program, avatarID, nAvatars, difficulty, IP address, MazePort, logFile
    - Keep threads open during duration of Maze Solving functions
6. Once Maze is solved, or if other reasons to quit, free thread array and other allocated space.


### Avatar Logic:
1. Start socket connection for the avatar through the MazePort (from initial server message)
2. Initialize and send ready message
3. Begin maze solving sequence:
    - Recieve message from server with avatar locations/first turn ID
    -  Check the status of the message (maze solved/error/maze not solved)
    - If maze is solved, or error, return.
    -  Otherwise, attempt to move avatar by calling `choose_next_move`, which chooses the move for the avatar and creates/returns the message to send to the server. We then send that message to the server.


### Avatar Movement Logic/Pseudocode:
- "Right Wall Following": Avatar moves in a straight line as far as possible until it hits a wall, then tries to turn right.
    1. move_wallFollower takes in the maze, current (x,y) positions and the current direction. If the maze or current direction is NULL, we return -1 for error.
    2. Otherwise, we allocate space for the next XYPos, set an indicator boolean to true.
    3. Depending on the input direction, we switch the direction to whichever direction would then correspond to a right turn
    4. Then, while our indicator boolean (flag) is true, and the number of turns is less than 5, we update the next coordinates for the avatar. We then check that the movement designated as next is valid (from `maze.c`) and if it is, we change the indicator boolean (flag) to false. Otherwise we increment the number of turns and change the direction that the avatar is facing.


### Maze Tracking Logic/Pseudocode:
- The `maze` struct is contained within each avatar struct and has the height and width of the maze, as well as information for all the maze pieces (which track the locations of discovered walls).
- Whenever an avatar hits a wall (known by the message from the server), the maze is updated to include a wall at that location.
- If an avatar hits a deadend, a dead end wall is placed to prevent other avatars from exploring that location, without trapping avatars that may be already in the deadend (it allows for travel out of the deadend).
- Before an avatar moves, it checks if it is valid by checking if there is a wall at the location it wants to move in the `maze` struct.


### General Maze Solving Algorithm:
- The logic for the algorithm involves a "right-hand" approach. This approach mimics the idea that, in a maze, following the maze with the right hand touching the wall will eventually lead to "solving" the maze. In other words, the avatars will turn 90 degrees clockwise with every dead-end (keeping their "right hand" on the wall). Eventually, the avatars will have explored the maze to some extent and will end up all at the same position.


### Optimization:
1. We have implemented a "dead-end blocking" optimization approach. With each discovered dead-end, the program will block off the area so that avatars exploring the region later can avoid exploring dead ends.
2.


### Data Logging:
Log files will be stored in a sub-directory where the client is run from.
1. Log maze initialization, including # of avatars and difficulty.
2. Document each thread and relevant data
3. Indicate start of solve
4. On each avatar's turn:
    1. Avatar # at (x,y) requests to move "direction"
    2. Record if successful and new location (or same)
    3. Recording each message received and sent
5. When maze is solved:
    1. Record number of avatars, difficulty, number of moves, and hash value
        - Delivered from server, indicates completion.
    2. Print maze to logfile


### GUI:
The GUI program for the maze was written in Java. This program reads from the file called "guiThing.txt". This file stores the contents of the maze in 0's and 1's. Parsing through this file allows the GUI program to draw out what the maze looks like, updating the interface with each avatar's movement and new walls that are discovered.


### Testing:
1. Test each level with varying number of avatars
2. Test with varyious correct and incorrect inputs (invalid server name, out of bounds level/number of avatars, missing arguments, extra arguments)
3. Further info in TESTING.md
