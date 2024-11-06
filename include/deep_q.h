#ifndef DEEP_Q_H
#define DEEP_Q_H

#include "game.h"
#include "minimax.h"
#include <tensorflow/c/c_api.h>

// add constants so the state of tensorflow is preserved in memory throughout the execution of the program
extern TF_Graph* graph;
extern TF_Session* session;
extern TF_Status* status;

/// @brief loads the tensorflow model and initializes tensorflow into a state ready for inference
/// @param model_path path of the saved_model FOLDER
void init_tensorflow(const char* model_path);

/// @brief performs inference on the current board state, using the best q values to find the best move
/// @param board a copy of the tic tac toe board
/// @param currentPlayer unused, for compatibility with minimax's findBestMove
/// @param playerStartFirst unused, for compatibility with minimax's findBestMove,
/// @return 
Pair findBestDLMove(int board[3][3], PlayerType currentPlayer, bool playerStartFirst);

/// @brief cleans up the resources used by tensorflow to prevent memory leaks
void cleanup_tensorflow();

#endif // DEEP_Q_H