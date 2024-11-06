#include <gtk/gtk.h>
#include <include/minimax.h>
#include <include/gui.h>
#include <include/game.h>
#include <stdbool.h>
#include <string.h>
#include <include/definitions.h>
#include <include/deep_q.h>

// Define the GUI elements
GtkWidget *window;
GtkWidget *grid;
GtkWidget *buttons[3][3];
GtkWidget *mode_combo_box;
GtkWidget *difficulty_combo_box;
GtkWidget *undo_button;
GtkWidget *redo_button;
GtkWidget *surrender_button;
GtkWidget *restart_button;
GtkWidget *start_button;

PlayerType opponent = AI;
bool aiIsDeepLearning = false;

// Function to refresh the grid
static void refresh_grid() {
  for (int i = 0; i < 3; i++) {
    for (int j = 0; j < 3; j++) {
      gtk_widget_set_sensitive(buttons[i][j], gameState.board[i][j] == 0 && gameState.isStarted == TRUE);
      if (gameState.board[i][j] == BOARD_CROSS) {
        gtk_button_set_label(GTK_BUTTON(buttons[i][j]), "X");
      } else if (gameState.board[i][j] == BOARD_NOUGHT) {
        gtk_button_set_label(GTK_BUTTON(buttons[i][j]), "O");
      } else {
        gtk_button_set_label(GTK_BUTTON(buttons[i][j]), "");
      }
    }
  }
}

// Refreshes the buttons to keep it up-to-date with current game
static void refresh_buttons(){
    if(gameState.winner == UNASSIGNED && gameState.isStarted && !gameState.isDraw){
        gtk_widget_set_sensitive(start_button, FALSE);
        gtk_widget_set_sensitive(restart_button, FALSE);
        gtk_widget_set_sensitive(difficulty_combo_box, FALSE);
        gtk_widget_set_sensitive(mode_combo_box, FALSE);

        // enable game buttons
        gtk_widget_set_sensitive(undo_button, TRUE);
        gtk_widget_set_sensitive(redo_button, TRUE);
        gtk_widget_set_sensitive(surrender_button, TRUE);
    }else if(gameState.winner != UNASSIGNED || gameState.isDraw){
        gtk_widget_set_sensitive(restart_button, TRUE);
        gtk_widget_set_sensitive(difficulty_combo_box, TRUE);
        gtk_widget_set_sensitive(mode_combo_box, TRUE);

        // disable game only buttons
        gtk_widget_set_sensitive(undo_button, FALSE);
        gtk_widget_set_sensitive(redo_button, FALSE);
        gtk_widget_set_sensitive(surrender_button, FALSE);
    }
}

void startGame(){
    // Initialize the game state
    createGameState(opponent);
    if(!gameState.player1StartFirst){
        
        // do ai move first
        if(gameState.opponent == AI)
            do_ai_move();
        
        refresh_grid();
    }
    // disable the start and restart buttons
    refresh_buttons();
}

// Function to handle win/draw logic win_draw true = win, false = draw
static void handle_win_draw() {
    refresh_grid();
    // Disable all buttons
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            gtk_widget_set_sensitive(buttons[i][j], FALSE);
        }
    }

    // Show the result
    if (!gameState.isDraw) {
        if (gameState.opponent == PLAYER_2) {
            GtkWidget *dialog = gtk_message_dialog_new(
                GTK_WINDOW(window), GTK_DIALOG_DESTROY_WITH_PARENT, GTK_MESSAGE_INFO,
                GTK_BUTTONS_CLOSE, "Game Over! Player %d won!", gameState.winner + 1);
            gtk_dialog_run(GTK_DIALOG(dialog));
            gtk_widget_destroy(dialog);
        } else if (gameState.opponent == AI) {
            GtkWidget *dialog = gtk_message_dialog_new(
                GTK_WINDOW(window), GTK_DIALOG_DESTROY_WITH_PARENT, GTK_MESSAGE_INFO,
                GTK_BUTTONS_CLOSE,
                gameState.winner == PLAYER_1 ? "Game Over! You Win!"
                                            : "Game Over! You lose!");
            gtk_dialog_run(GTK_DIALOG(dialog));
            gtk_widget_destroy(dialog);
        }

    } else {
        GtkWidget *dialog = gtk_message_dialog_new(
            GTK_WINDOW(window), GTK_DIALOG_DESTROY_WITH_PARENT, GTK_MESSAGE_INFO,
            GTK_BUTTONS_CLOSE, "Game Over! Draw!");
        gtk_dialog_run(GTK_DIALOG(dialog));
        gtk_widget_destroy(dialog);
    }
    // update the ui
    refresh_buttons();
}

// Function to handle button clicks
static void button_clicked(GtkWidget *widget, gpointer data) {
  // Get the row and column of the clicked button
  int row = GPOINTER_TO_INT(data) / 3;
  int col = GPOINTER_TO_INT(data) % 3;

  // Do the move
  bool move_success = doMove(row, col);
  if (move_success) {
    nextTurn();
    gtk_widget_set_sensitive(widget, FALSE);
    refresh_grid();
    
    // Check for win or draw
    if (gameState.winner != UNASSIGNED || gameState.isDraw) {
      handle_win_draw();
      return;          

    }

    if(gameState.opponent == AI)
        do_ai_move();
  }
}

void do_ai_move(){
    // AI move
    if (gameState.opponent == AI && gameState.winner == UNASSIGNED && !gameState.isDraw) {
      int t_board[3][3];

      // find the best move with a copy of the array, in order to avoid modifying the current array (pass by ref)
      memcpy(t_board, gameState.board, sizeof(gameState.board));
      Pair pair;
      if(aiIsDeepLearning){
        pair = findBestDLMove(t_board, gameState.turn, gameState.player1StartFirst);
      }else{
        pair = findBestMove(t_board, gameState.turn, gameState.player1StartFirst);
      }
      doMove(pair.a, pair.b);
      nextTurn();
      gtk_widget_set_sensitive(buttons[pair.a][pair.b], FALSE);
    }

    // Check for win or draw after AI move
    if (checkWin() || checkDraw()) 
        handle_win_draw();

    refresh_grid();
}

// Function to handle mode combo box changes
static void mode_combo_box_changed(GtkWidget *widget, gpointer data) {
    // Get the selected mode
    int mode = gtk_combo_box_get_active(GTK_COMBO_BOX(widget));

    // Enable or disable the difficulty combo box based on the selected mode
    gtk_widget_set_sensitive(difficulty_combo_box, mode == 0 || mode == 2);

    switch(mode){
        case 0:
            opponent = AI;
            aiIsDeepLearning = false;
            break;
        case 1:
            opponent = PLAYER_2;
            break;
        case 2:
            opponent = AI;
            aiIsDeepLearning = true;
            break;
    }

}

// Function to handle difficulty combo box changes
static void difficulty_combo_box_changed(GtkWidget *widget, gpointer data) {
    // Get the selected difficulty
    int difficulty = gtk_combo_box_get_active(GTK_COMBO_BOX(widget));

    //Set the maxDepth for the minimax algorithm based on the selected difficulty

    if (difficulty == 0)
        MAX_DEPTH = 2;
    else
        MAX_DEPTH = 9;
    
}

// Function to handle undo button clicks
void undo_button_clicked(GtkWidget *widget, gpointer data) {
    undo();
    refresh_grid();
}

// Function to handle redo button clicks
void redo_button_clicked(GtkWidget *widget, gpointer data) {
    redo();
    refresh_grid();
}

// Function to handle surrender button clicks
static void surrender_button_clicked(GtkWidget *widget, gpointer data) {
    // Determine the winner
    gameState.winner =
        (gameState.turn == gameState.player) ? gameState.opponent : gameState.player;

    // Show the result
    GtkWidget *dialog = gtk_message_dialog_new(
        GTK_WINDOW(window), GTK_DIALOG_DESTROY_WITH_PARENT, GTK_MESSAGE_INFO,
        GTK_BUTTONS_CLOSE, "Game Over! %s won!",
        gameState.winner == PLAYER_1 ? "Player 1" : "Player 2");
    gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);

    refresh_buttons();
}

// Function to handle start button clicks
static void start_button_clicked(GtkWidget *widget, gpointer data) {
    // Reset the button labels and sensitivities
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            gtk_button_set_label(GTK_BUTTON(buttons[i][j]), "");
            gtk_widget_set_sensitive(buttons[i][j], TRUE);
        }
    }

    // disable the start button
    gtk_widget_set_sensitive(start_button, FALSE);
    startGame();
    return;
}

// Function to handle restart button clicks
static void restart_button_clicked(GtkWidget *widget, gpointer data) {
    // Reset the game state
    destroyGameState();
    start_button_clicked(widget, data);
}

// Function to update font size based on button size
static void update_font_size(GtkWidget *widget, GdkRectangle *allocation, gpointer user_data) {
  // Get the button's height
  int button_height = allocation->height;

  // Calculate font size based on button height, half is pretty good and makes it NICE
  int font_size = button_height / 2;

  // Create a PangoFontDescription
  PangoFontDescription *font_desc = pango_font_description_new();
  pango_font_description_set_absolute_size(font_desc, font_size * PANGO_SCALE);

  // Apply the font to the button's label
  gtk_widget_override_font(widget, font_desc);

  // Free the font description
  pango_font_description_free(font_desc);
}

// Function to create the main window
static void activate(GtkApplication *app, gpointer user_data) {
    // Create the main window
    window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(window), "Tic Tac Toe");
    gtk_window_set_default_size(GTK_WINDOW(window), 300, 400);

    // Create a vertical box to hold the grid
    GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);  // 5 is spacing between elements
    gtk_container_add(GTK_CONTAINER(window), vbox);

    // Create the grid
    grid = gtk_grid_new();
    gtk_box_pack_start(GTK_BOX(vbox), grid, TRUE, TRUE, 0); 

    // Create grid buttons
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            buttons[i][j] = gtk_button_new();
            // let the button expand with window
            gtk_widget_set_hexpand(buttons[i][j], TRUE);
            gtk_widget_set_vexpand(buttons[i][j], TRUE);

            gtk_widget_set_size_request(buttons[i][j], 200, 200);
            gtk_grid_attach(GTK_GRID(grid), buttons[i][j], i, j, 1, 1);
            // connect with the on click function
            g_signal_connect(buttons[i][j], "clicked", G_CALLBACK(button_clicked),
                            GINT_TO_POINTER(i * 3 + j));

            // Connect to "size-allocate" signal to update font size on resize
            g_signal_connect(buttons[i][j], "size-allocate", G_CALLBACK(update_font_size), buttons[i][j]);
        }
    }

    // Create the mode combo box
    GtkWidget *mode_label = gtk_label_new("Opponent:");
    gtk_grid_attach(GTK_GRID(grid), mode_label, 0, 3, 1, 1);
    mode_combo_box = gtk_combo_box_text_new();
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(mode_combo_box),
                                    "AI Minimax");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(mode_combo_box), "1v1 Human");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(mode_combo_box),
                                    "Qlearning AI");
    gtk_grid_attach(GTK_GRID(grid), mode_combo_box, 1, 3, 2, 1);
    g_signal_connect(mode_combo_box, "changed",
                    G_CALLBACK(mode_combo_box_changed), NULL);

    // Create the difficulty combo box
    GtkWidget *difficulty_label = gtk_label_new("Difficulty:");
    gtk_grid_attach(GTK_GRID(grid), difficulty_label, 0, 4, 1, 1);

    difficulty_combo_box = gtk_combo_box_text_new();
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(difficulty_combo_box),
                                    "Easy");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(difficulty_combo_box),
                                    "Impossible");
    gtk_grid_attach(GTK_GRID(grid), difficulty_combo_box, 1, 4, 2, 1);
    g_signal_connect(difficulty_combo_box, "changed",
                    G_CALLBACK(difficulty_combo_box_changed), NULL);

    // Create the undo button
    undo_button = gtk_button_new_with_label("Undo");
    gtk_grid_attach(GTK_GRID(grid), undo_button, 0, 5, 1, 1);
    g_signal_connect(undo_button, "clicked", G_CALLBACK(undo_button_clicked),
                    NULL);

    // Create the redo button
    redo_button = gtk_button_new_with_label("Redo");
    gtk_grid_attach(GTK_GRID(grid), redo_button, 1, 5, 1, 1);
    g_signal_connect(redo_button, "clicked", G_CALLBACK(redo_button_clicked),
                    NULL);

    // Create the surrender button
    surrender_button = gtk_button_new_with_label("Surrender");
    gtk_grid_attach(GTK_GRID(grid), surrender_button, 2, 5, 1, 1);
    g_signal_connect(surrender_button, "clicked",
                    G_CALLBACK(surrender_button_clicked), NULL);

    // Create the restart button
    restart_button = gtk_button_new_with_label("Restart");
    gtk_grid_attach(GTK_GRID(grid), restart_button, 0, 6, 3, 1);
    g_signal_connect(restart_button, "clicked",
                    G_CALLBACK(restart_button_clicked), NULL);
    
    // Create the start button
    start_button = gtk_button_new_with_label("Start Game");
    gtk_grid_attach(GTK_GRID(grid), start_button, 0, 10, 3, 4);
    g_signal_connect(start_button, "clicked",
                    G_CALLBACK(restart_button_clicked), NULL);

    // Show all widgets
    gtk_widget_show_all(window);

    // set the first options as the defaults
    gtk_combo_box_set_active(GTK_COMBO_BOX(difficulty_combo_box), 0);
    gtk_combo_box_set_active(GTK_COMBO_BOX(mode_combo_box), 0);

    refresh_grid();
    refresh_buttons();
}

void launch_gui(int argc, char **argv) {
    // Create the GTK application
    GtkApplication *app = gtk_application_new("com.kkxln.tictactoe", G_APPLICATION_DEFAULT_FLAGS);
    g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);
    int status = g_application_run(G_APPLICATION(app), argc, argv);
    g_object_unref(app);
}
