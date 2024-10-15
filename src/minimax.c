#include <include/minimax.h>

int evaluateBoard(int board[3][3]) {
    // Check rows
    for (int i = 0; i < 3; i++) {
        if (board[i][0] == board[i][1] && board[i][1] == board[i][2]) {
            if (board[i][0] == 1) return 10; // Player A wins
            if (board[i][0] == 2) return -10; // Player B (AI) wins
        }
    }

    // Check columns
    for (int j = 0; j < 3; j++) {
        if (board[0][j] == board[1][j] && board[1][j] == board[2][j]) {
            if (board[0][j] == 1) return 10;
            if (board[0][j] == 2) return -10;
        }
    }

    // Check diagonals
    if ((board[0][0] == board[1][1] && board[1][1] == board[2][2]) ||
        (board[0][2] == board[1][1] && board[1][1] == board[2][0])) {
        if (board[1][1]  
 == 1) return 10;
        if (board[1][1] == 2) return -10;
    }

    return 0; // No winner yet
}

int minimax(int board[3][3], int depth, bool isMaximizing, PlayerType currentPlayer) {
    int score = evaluateBoard(board);

    if (score == 10) {
        return score - depth; // Player A wins (minimize depth)
    }
    if (score == -10) {
        return score + depth; // Player B (AI) wins (maximize depth)
    }

    if (!isMovesLeft(board)) {
        return 0; // Draw
    }

    if (isMaximizing) { // AI's turn (Player B)
        int best = -1000;
        for (int i = 0; i < 3; i++) {
            for (int j = 0; j < 3; j++) {
                if (board[i][j] == 0) {
                    board[i][j] = (currentPlayer == PLAYER_1) ? 1 : 2; // AI's symbol
                    best = max(best, minimax(board, depth + 1, !isMaximizing, 
                                            (currentPlayer == PLAYER_1) ? AI : PLAYER_1));
                    board[i][j] = 0; 
                }
            }
        }
        return best;
    } else { // Human player's turn (Player A)
        int best = 1000;
        for (int i = 0; i < 3; i++) {
            for (int j = 0; j < 3; j++) {
                if (board[i][j] == 0) {
                    board[i][j] = (currentPlayer == PLAYER_1) ? 1 : 2; // Human's symbol
                    best = min(best, minimax(board, depth + 1, !isMaximizing, 
                                            (currentPlayer == PLAYER_1) ? AI : PLAYER_1));
                    board[i][j] = 0; 
                }
            }
        }
        return best;
    }
}

Pair findBestMove(int board[3][3], PlayerType currentPlayer) {
    int bestVal = -1000;
    Pair bestMove;
    bestMove.a = -1;
    bestMove.b = -1;

    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            if (board[i][j] == 0) {
                board[i][j] = (currentPlayer == PLAYER_1) ? 2 : 1; // AI's symbol
                int moveVal = minimax(board, 0, false, (currentPlayer == PLAYER_1) ? AI : PLAYER_1);
                board[i][j] = 0; 

                if (moveVal > bestVal) {
                    bestMove.a = i;
                    bestMove.b = j;
                    bestVal = moveVal;
                }
            }
        }
    }
    return bestMove;
}