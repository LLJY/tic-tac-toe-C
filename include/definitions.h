// This file contains the standard includes, and macro definitions
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdarg.h>
#include <ctype.h>
#include <math.h> 

#ifndef DEFINITIONS
#define DEFINITIONS
// built in branch predictor hints for gcc, these tell GCC how to optimize the code better
// and for supported CPUs, it can tell the branch predictor to preload the most likely codepath
#if defined(__GNUC__) || defined(__clang__)

#define likely(x)      __builtin_expect(!!(x), 1)
#define unlikely(x)    __builtin_expect(!!(x), 0)

#else

// Fallback definitions if the compiler doesn't support __builtin_expect
#define likely(x)      (x)
#define unlikely(x)    (x)

#endif

/// @brief Player type is the definition for player in integers
/// It removes magic numbers from the code and makes it clearer which player is which.
/// it is mainly used for game logic to determine the current player and opponent.
typedef enum PlayerType{
    PLAYER_1 = 0,
    PLAYER_2 = 1,
    AI = -1,
    UNASSIGNED = -99
}PlayerType;

// Empty definition for the ttt board.
#define BOARD_EMPTY 0
// Cross (X) definition for the ttt board.
#define BOARD_CROSS 1
// Nought (O) definition for the ttt board.
#define BOARD_NOUGHT 2
// Global variable to store the maximum minimax depth, affects difficulty.
extern int MAX_DEPTH;
#endif