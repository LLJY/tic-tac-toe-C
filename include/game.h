#include <util.h>
#include <linked_list.h>

#ifndef GAME_H
#define GAME_H

/// @brief The whole state of the game, individual parameters are documented in the header.
typedef struct GameState{
    int board[3][3];// 2D array for ttt board, 0 for empty, 1 for X (Cross), 2 for nought (O)
    Node* currentMove; // head of the linked list
    bool player1StartFirst; // randomly initialised variable, X will always be drawn first, so this is used to determine whether or not it is AI or player that starts first.
    bool isDraw; // handle draw cases, as they are a special case.
    bool isStarted; // states if the game has started (for gui only). not required for tui since it terminates upon game termination.
    PlayerType winner; // winner will be UNNASSIGNED until somebody actually wins.
    PlayerType turn; // 0 for player 1, 1 for player 2, -1 for AI
    PlayerType player; // Player 1 will always be a human player
    PlayerType opponent; // Player 2 can be AI or player.
}GameState;

/// @brief stores the gameState in a global variable.
extern GameState gameState;

/// @brief Creates an initializes gameState into a ready state
extern void createGameState();

/// @brief destroys gameState to save memory, to be executed at the end by the agent.
extern void destroyGameState();

/// @brief Executes the current player move in column and row
/// @param col column of the move
/// @param row row of the move
/// @return returns true if successful, false if disallowed.
bool doMove(int col, int row);

/// @brief Checks if the game has been won on any win condition
/// @return 
bool checkWin();

/// @brief Checks if the game has no possible ways of winning
/// @return 
bool checkDraw();

/// @brief Checks for win and draw conditions and updates GameState Accordingly
/// @return 
void nextTurn();

/// @brief Checks if there are possible moves left, return true, otherwise, return false
/// @return 
bool isMovesLeft();

/// @brief Traverses backwards in the linked list, in order to undo a previous turn. Resets gameState automatically and prevents repeated turns.
void undo();

/// @brief Traverses forwards in the linked list, in order to redo a previous turn. Resets gameState automatically and prevents repeated turns.
void redo();
#endif