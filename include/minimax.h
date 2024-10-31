#include <stdbool.h>
#include <math.h>
#include <util.h>

typedef struct Pair {
    int a;
    int b;
} Pair;

/// @brief Checks the board and returns a score.
/// @param board The tic-tac-toe board.
/// @return 10 if AI wins -10 if player wins, 0 otherwise
int evaluateBoard(int board[3][3]);

/// @brief The minimax function (recursive).
/// @param board The tic-tac-toe board.
/// @param depth How deep we are in the search
/// @param isMaximizing True if it's the AI's turn, false if it's the player's turn.
/// @return The best score the current player can get
int minimax(int board[3][3], int depth, bool isMaximizing, PlayerType currentPlayer, int maxDepth);

/// @brief Figures out the best move for the AI.
/// @param board The tic-tac-toe board.
/// @return The best move the AI can make.
Pair findBestMove(int board[3][3], PlayerType currentPlayer);

/// @brief Checks if there are any empty spots left on the board
/// @param board The tic-tac-toe board.
/// @return True if there are empty spots, false otherwise
bool isMovesLeft(int board[3][3]);