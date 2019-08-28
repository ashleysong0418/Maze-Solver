# TESTING
## Group Anon, CS50 19X
## Rylee Stone, Ashley Song, Kyle Bensink, Barrett Noone



### Testing Plan:
1. Test lower levels (0-4) with various numbers of avatars without the GUI
2. Test higher levels (5-9) with various numbers of avatars without the GUI
3. Test lower levels (0-4) with various numbers of avatars with the GUI
4. Test higher levels (5-9) with various numbers of avatars with the GUI
5. Test with various correct and incorrect inputs (invalid server name, out of bounds level/number of avatars, missing arguments, extra arguments)

### Testing Output:
- See logFiles for detailed breakdown of program executions for cases
- With incorrect output, program exits with error code and prints error statement

> With invalid host name:
```
./client 1 1 moose
Invalid hostname
```

> With too few arguments:
```
./client 1 1
Usage: ./client <nAvatars> <difficulty (0-9)> <hostname>
```

> With too many arguemnts:
```
./client 1 1 moose.cs.dartmouth.edu extra
Usage: ./client <nAvatars> <difficulty (0-9)> <hostname>
```

> With out of bounds nAvatars:
```
./client -1 1 moose.cs.dartmouth.edu
Error: initialization failed
```

> With out of bounds difficulty:
```
./client 3 10 moose.cs.dartmouth.edu
Error: initialization failed
```
