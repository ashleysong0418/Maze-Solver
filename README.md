# The Maze Challenge

## Overview: 
The Maze Solver is a program created to take a maze from the server and "solve" it using an algorithm. Please read the DESIGN.md for further explanation on this project and its implementation. 

### Directory Structure:
1. avatar.[ch]
2. maze.[ch]
3. movement.[ch]
4. functions.[ch]
5. libcs50/
6. common.h
7. logFiles/
    * log files printed for each trial
8. guiThing.txt (maze logging file)
9. GUI.java
10. DESIGN.md
11. IMPLEMENTATION.md

### Assumptions:
> The host will always be moose.cs.dartmouth.edu

### Compiling and Running:
> Download XQuartz terminal
> Open XQuartz, go to "Applications" and select "terminal"
> log into server with ssh -Y user@moose.cs.dartmouth.edu
> use `make` to compile client.c program
> use `make withGui` to compile Java GUI
> Run with: ./client <number of avatars> <difficulty> moose.cs.dartmouth.edu
> use `make clean` to clean up all compiled files 
   
   
### Example Usage:
Input: ./client 4 4 moose.cs.dartmouth.edu
Output:
![Terminal Output](https://github.com/ashleysong0418/Maze-Solver/blob/master/img/outcome.png)
![GUI Output] (https://github.com/ashleysong0418/Maze-Solver/blob/master/img/GUI.jpg)

