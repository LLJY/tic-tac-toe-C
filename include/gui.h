#ifndef GUI_H
#define GUI_H

/// Global variable to define whether or not to use the tensorflow ai. Defined and controlled by gui.c
extern bool aiIsDeepLearning;

/// @brief Entry point for the gui, scaffolds and initializes the gui.
/// @param argc arguments from C, not super important, can just directory pass over from main function, let gtk parse and handle the argments.
/// @param argv arguments from C, not super important, can just directory pass over from main function, let gtk parse and handle the argments.
extern void launch_gui(int argc, char **argv);

/// @brief executes the move for the AI. Here we do a conditional check to decided whether or not we should use minimax or tensorflow.
void do_ai_move();
#endif