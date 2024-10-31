#include <include/game.h>

GameState gameState;

void createGameState(PlayerType opponent){
    // Initialize the board to all zeros (empty)
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            gameState.board[i][j] = BOARD_EMPTY;
        }
    }

    gameState.player1StartFirst = rand() % 2;

    gameState.opponent = opponent;

    // since there is no first move, leave the linkedlist and winner as empty

    gameState.currentMove = NULL;

    gameState.winner = UNASSIGNED;

    // initialize the first turn
    if(gameState.player1StartFirst){
        gameState.turn = PLAYER_1;
    }else{
        gameState.turn = opponent;
    }
}

void destroyGameState(){
    if(gameState.currentMove != NULL){
        destroyList(gameState.currentMove);
    }
}

bool doMove(int row, int col){
    int insertChar;
    if(gameState.player1StartFirst){
        if(gameState.turn == AI || gameState.turn == PLAYER_2){
            insertChar = BOARD_NOUGHT;
        }else{
            insertChar = BOARD_CROSS;
        }
    }else{
        if(gameState.turn == AI || gameState.turn == PLAYER_2){
            insertChar = BOARD_CROSS;
        }else{
            insertChar = BOARD_NOUGHT;
        }
    }

    // if the move is illegal, and attempts to replace an existing move, disallow it.
    if(gameState.board[row][col] != BOARD_EMPTY)
        return false;

    gameState.board[row][col] = insertChar;

    Node* currentMoveNode = createNode(gameState.turn, row, col, NULL, NULL);

    // make use of branch predictor hints here to increase performance during QL training.
    if(likely(gameState.currentMove != NULL)){ 
        if(unlikely(gameState.currentMove->Next != NULL)){
            Node* listNode = gameState.currentMove;  //remove the NEXT node until there are no more moves in the linked list so as to not break REDO and UNDO
            while (listNode->Next != NULL)
            {
                Node* t = listNode->Next;
                free(t->Prev);
                listNode = t;
            }
            free(listNode); // the loop will traverse until the last node, but it won't free the last node.

            // set the current move's next pointer to NULL
            gameState.currentMove->Next = NULL;
        }
        insertNodeHead(&gameState.currentMove, currentMoveNode);
    }else{
        gameState.currentMove = currentMoveNode;
    }
    return true;
}

bool checkDraw(){
    if(unlikely(!isMovesLeft(gameState.board))){
        gameState.isDraw = true;
        return true;
    }
    // if there's no win, check for draw
    int drawCount = 0;

    // create a temp list to avoid unnecessary memory allocations
    int t[3] = {0, 0, 0};
    int a;
    int b;
    int c;
    int numX;
    int numO;

    /* So far this is the *second* most efficient method I can think of to early draw detection
    The MOST efficient would be to represent each O and X as a prime number, then use
    prime factorization to count the number of O and X using modulo. */

    // check for line (x-x-x) draws
    for(int i = 0; i < 3; i++){
        numX = 0;
        numO = 0;
        a = gameState.board[i][0];
        b = gameState.board[i][1];
        c = gameState.board[i][2];
        t[0] = a; t[1] = b; t[2] = c;
        for(int j = 0; j < 3; j++){
            if(t[j] == BOARD_CROSS){
                numO++;
            }else if(t[j] == BOARD_NOUGHT){
                numX++;
            }
        }
        if(numO > 0 && numX > 0){
            drawCount++;
        }
    }

    // check for cross draws
    a = gameState.board[0][0];
    b = gameState.board[1][1];
    c = gameState.board[2][2];
    numX = 0;
    numO = 0;
    t[0] = a; t[1] = b; t[2] = c;
    for(int j = 0; j < 3; j++){
        if(t[j] == BOARD_CROSS){
            numO++;
        }else if(t[j] == BOARD_NOUGHT){
            numX++;
        }
    }
    if(numO > 0 && numX > 0){
        drawCount++;
    }

    // check for cross draws
    a = gameState.board[2][0];
    b = gameState.board[1][1];
    c = gameState.board[0][2];
    numX = 0;
    numO = 0;
    t[0] = a; t[1] = b; t[2] = c;
    for(int j = 0; j < 3; j++){
        if(t[j] == BOARD_CROSS){
            numO++;
        }else if(t[j] == BOARD_NOUGHT){
            numX++;
        }
    }
    if(numO > 0 && numX > 0){
        drawCount++;
    }            

    for(int i = 0; i < 3; i++){
        numX = 0;
        numO = 0;
        a = gameState.board[0][i];
        b = gameState.board[1][i];
        c = gameState.board[2][i];
        t[0] = a; t[1] = b; t[2] = c;
        for(int j = 0; j < 3; j++){
            if(t[j] == BOARD_CROSS){
                numO++;
            }else if(t[j] == BOARD_NOUGHT){
                numX++;
            }
        }
        if(numO > 0 && numX > 0){
            drawCount++;
        }                
    }
    if(drawCount == 8){
        return true;
    }
    return false;
}

bool checkWin(){
    bool win = false;
    int cmp; // the comparator to compare against

    // check for line (x-x-x) wins
    for(int i = 0; i < 3; i++){
        cmp = gameState.board[i][0];
        if(cmp == 0) // skip this loop if cmp is 0, which is not worth comparing
            continue;
        win = (gameState.board[i][1] == cmp && gameState.board[i][2] == cmp) || win;
    }

    // check for cross wins
    cmp = gameState.board[0][0];
    win = (cmp != 0 && (gameState.board[1][1] == cmp && gameState.board[2][2] == cmp)) || win;

    cmp = gameState.board[2][0];
    win = (cmp != 0 && (gameState.board[1][1] == cmp && gameState.board[0][2] == cmp)) || win;

    // check for vertical line wins
    for(int i = 0; i < 3; i++){
        cmp = gameState.board[0][i];
        if(cmp == 0) // skip this loop if cmp is 0, which is not worth comparing
            continue;
        win = (gameState.board[1][i] == cmp && gameState.board[2][i] == cmp) || win;
    }
    return win;
}

void nextTurn(){
    bool win = checkWin();
    if(win){
        // the winner can only be the one that is currently playing the turn.
        gameState.winner = gameState.turn;
    }else{
        gameState.isDraw = checkDraw();
    }
    gameState.turn = gameState.turn == gameState.player ? gameState.opponent : gameState.player;
}

bool isMovesLeft(int board[3][3]) {
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            if (board[i][j] == BOARD_EMPTY) {
                return true;
            }
        }
    }
    return false;
}

void undo();

void redo();