#include <util.h>

/// @brief Shows the end of game ui, handles losses, wins and surrenders.
// The game will terminate after.
void endGameUi();

/// @brief prompts the user to select the opponent they will be playing against. Contains error handling for invalid inputs.
/// @return returns a PlayerType corresponding to the opponent the player is playing against.
PlayerType selectOpponentTypeUi();

/// @brief Prompts the user for the next move (e.g. redo, undo, surrender, etc). Contains error handling for invalid moves.
void selectMoveUi();

/// @brief refreshes the terminal ui with the latest tic tac toe board state. Row = A,B,C Col = 1,2,3
void refreshUi();

/// @brief Loads the start game ui to prompt the user to select opponent type and difficulty. Contains error handling for invalid inputs.
void startGameUi();
