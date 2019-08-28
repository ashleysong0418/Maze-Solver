/* ===============================================================
 * common.h:
 * header file defining constants and data types used by
 * the maze project.
 *
 * CS50, Summer 2019
 * =============================================================== */

#ifndef __COMMON_H
#define __COMMON_H

/*********** Prerequisites, e.g., math.h ************/
#include <stdint.h>             // uint32_t

/*********** Constants ************/
/* Server parameters */
#define SERVER_PORT   17235    // server management port, ASCII "CS"
#define MAX_MESSAGE     128     // max message length in bytes
#define MAX_DIFFICULTY    9     // max # of maze difficulty levels
#define MAX_AVATAR       10     // max # of avatars for any MazePort
#define MAX_MOVES      1000     // constant used to compute max # of moves
#define WAIT_TIME       600     // seconds waited before server dies

/* Avatar constants */
#define M_WEST           0
#define M_NORTH          1
#define M_SOUTH          2
#define M_EAST           3
#define M_NUM_DIRECTIONS 4
#define M_NULL_MOVE      8

/* Client-server protocol message type values */
#define ERROR_MASK             0x80000000
#define MAZE_SOLVED            0x00010000
#define INIT                   0x00000001
#define INIT_OK                0x00000002
#define AVATAR_READY           0x00000004
#define AVATAR_TURN            0x00000008
#define AVATAR_MOVE            0x00000010
#define INIT_FAILED           (0x00000020 | ERROR_MASK)
#define AVATAR_OUT_OF_TURN    (0x00000040 | ERROR_MASK)
#define NO_SUCH_AVATAR        (0x00000080 | ERROR_MASK)
#define TOO_MANY_MOVES        (0x00000100 | ERROR_MASK)
#define SERVER_DISK_QUOTA     (0x00000200 | ERROR_MASK)
#define SERVER_OUT_OF_MEM     (0x00000400 | ERROR_MASK)
#define UNKNOWN_MSG_TYPE      (0x01000000 | ERROR_MASK)
#define SERVER_TIMEOUT        (0x02000000 | ERROR_MASK)
#define UNEXPECTED_MSG_TYPE   (0x04000000 | ERROR_MASK)

/* INIT failure ErrNums */
#define INIT_ERROR_MASK       (0x40000000 | ERROR_MASK)
#define INIT_TOO_MANY_AVATARS (0x00000001 | INIT_ERROR_MASK)
#define INIT_BAD_DIFFICULTY   (0x00000002 | INIT_ERROR_MASK)

/*********** Structures/Types ************/
/* XY-coordinate position */
typedef struct XYPos
{
    uint32_t x;
    uint32_t y;
} XYPos;

/* Maze avatar */
typedef struct Avatar
{
    int fd;
    XYPos pos;
} Avatar;

/* Message description */
typedef struct Message
{
    uint32_t type;

    /* Define a union for all the message types that have
     * parameters. Messages with no parameters don't need to be part
     * of this union. Defined as an anonymous union to facilitate easier access. */
    union
    {
        /* INIT */
        struct
        {
            uint32_t nAvatars;
            uint32_t Difficulty;
        } init;

        /* INIT_OK */
        struct
        {
            uint32_t MazePort;
            uint32_t MazeWidth;
            uint32_t MazeHeight;
        } init_ok;

        /* INIT_FAILED */
        struct
        {
            uint32_t ErrNum;
        } init_failed;

        /* AVATAR_READY */
        struct
        {
            uint32_t AvatarId;
        } avatar_ready;

        /* AVATAR_TURN */
        struct
        {
            uint32_t TurnId;
            XYPos    Pos[MAX_AVATAR];
        } avatar_turn;

        /* AVATAR_MOVE */
        struct
        {
            uint32_t AvatarId;
            uint32_t Direction;
        } avatar_move;

        /* MAZE_SOLVED */
        struct
        {
            uint32_t nAvatars;
            uint32_t Difficulty;
            uint32_t nMoves;
            uint32_t Hash;
        } maze_solved;

        /* UNKNOWN_MSG_TYPE */
        struct
        {
            uint32_t BadType;
        } unknown_msg_type;
    };
} Message;

/*********** Public Variables ************/
/* add your variables here if any*/

/*********** Prototypes/Macros ************/
#define IS_ERROR(code) ((code) & (ERROR_MASK))

#endif // __COMMON_H
