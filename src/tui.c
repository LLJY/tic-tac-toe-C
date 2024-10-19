#include <include/tui.h>
#include <include/minimax.h>
#include <include/game.h>
#include <string.h>

void endGameUi(){
    if(gameState.isDraw){
        println("Game Over! Draw!");
        return;
    }
    if(gameState.opponent == PLAYER_2){
        println("Game Over! Player %d has won!", gameState.winner + 1);
    }else if(gameState.opponent == AI){
        if(gameState.winner == PLAYER_1)
            println("Game Over! You Win!");
        else
            println("Game Over! You lose!");
    }
}

PlayerType selectOpponentTypeUi(){
    bool valid_input;
    int input;
    PlayerType opponent;
    while(!valid_input){
        println("Select Opponnent:");
        println("1. Multiplayer");
        println("2. AI Player (Minimax)");
        println("3. AI Player (Actual AI)");
        scanf("%d", &input);

        switch(input){
            case 1:
                valid_input = true;
                opponent = PLAYER_2;
                break;
            case 2:
                valid_input = true;
                opponent = AI;
                break;
            case 3:
                valid_input = true;
                opponent = AI;
                break;
            default:
                valid_input = false;
                break;
        }
    }
    gameState.player = PLAYER_1;
    clearScreen();
    return opponent;
}

void selectMoveUi(){
    bool option1_valid;
    char option1[2];
    int col;
    int row;
    while(!option1_valid){
        // input options
        println("Select Next Move");
        println("Valid Input example : \"A3\"");
        println("-----------------------------");
        scanf("%s", &option1);
        char colchar = toupper(option1[0]);
        char rowchar = option1[1];
        switch (colchar)
        {
        case 'A':
            col = 0;
            option1_valid = true;
            break;
        case 'B':
            col = 1;
            option1_valid = true;
            break;
        case 'C':
            col = 2;
            option1_valid = true;
            break;
        
        default:
            option1_valid = false;
            break;
        }

        if(!option1_valid)
            continue;

        switch (rowchar)
        {
        case '1':
            row = 0;
            option1_valid = true;
            break;
        case '2':
            row = 1;
            option1_valid = true;
            break;
        case '3':
            row = 2;
            option1_valid = true;
            break;
        default:
            option1_valid = false;
            break;
        }
    }
    bool moveSuccess = doMove(row, col);
    if(moveSuccess)
        nextTurn();
    else{
        println("Move disallowed");
        selectMoveUi();
    }
}

void refreshUi(){
    clearScreen();

    printf("\n"); // Add an initial newline for spacing
    printf("  1   2   3");
    printf("\n");

    // add the column values for ui prettyness
    for (int i = 0; i < 3; i++) {
        char row_char;
        switch(i){
            case 0:
                row_char = 'A';
                break;
            case 1:
                row_char = 'B';
                break;
            case 2:
                row_char = 'C';
                break;
        }
        printf("%c",row_char);
        for (int j = 0; j < 3; j++) {

            char character;
            switch(gameState.board[j][i]){
                case 0:
                    character = ' ';
                    break;
                case 1:
                    character = 'X';
                    break;
                case 2:
                    character = 'O';
                    break;
                default:
                    println("Game has entered an illegal state! exiting...");
                    exit(1);
            }
            // Center the character within its space
            printf(" %c ", character); 
            if (j < 2) {
                printf("|"); 
            }
        }
        printf("\n");
        if (i < 2) {
            printf("---+---+---\n"); 
        }
    }
    printf("\n");
    
    if(gameState.turn == PLAYER_1 || gameState.turn == PLAYER_2){
        int menu_option1;
        bool option1_valid = false;
        while(!option1_valid){
            // input options
            println("Player %d, You're up!", gameState.turn + 1);
            println("----- NEXT TURN OPTIONS -----");
            println("1) Execute Turn");
            println("2) Surrender");
            println("-----------------------------");
            scanf("%d", &menu_option1);
            switch (menu_option1)
            {
            case 1:
                selectMoveUi();
                option1_valid = true;
                break;
            
            case 2:
                gameState.winner = (gameState.turn == gameState.player) ? gameState.opponent : gameState.player;
                option1_valid = true;
                endGameUi();
                break;
            
            default:
                option1_valid = false;
                break;
            }
        }
    }else if(gameState.turn == AI){
        int t_board[3][3];
        memcpy(t_board, gameState.board, sizeof(gameState.board)); 
        Pair pair = findBestMove(t_board, gameState.turn);
        doMove(pair.a, pair.b);
        nextTurn();
    }
}

void startGameUi(){
    PlayerType opponent = selectOpponentTypeUi();
    createGameState(opponent);
    if(gameState.player1StartFirst){
        println("Player 1 Starts First, First Player Always X (Cross)");
    }else{
        println("Player 2 Starts First, First Player Always X (Cross)");
    }

    while (gameState.winner == UNASSIGNED && gameState.isDraw == false)
    {
        refreshUi();
    }

    endGameUi();
}
