// this file include utility functions that may be useful
#include <definitions.h>

#ifndef U_H
#define U_H

// add definitions for pretty text colour
#define BOLD_RED     "\033[1m\033[31m"
#define BOLD_GREEN   "\033[1m\033[32m"
#define BOLD_CYAN    "\033[1m\033[36m"
#define COLOUR_RESET   "\033[0m"

/// @brief println works exactly like printf except it adds a newline
/// This should avoid situations where you forget to print a newline.
/// @param format 
/// @param  
extern void println(const char *format, ...);


/// @brief Clears the terminal screen, will adapt to unix or windows.
void clearScreen();

/// @brief clears the input buffer, so that invalid inputs do not remain inside STDIN
/// When entering an invalid input into scanf, it causes an invalid input to stay in STDIN
/// because scanf errors out. Create a function to clear the input buffer upon errors.
void clearInputBuffer();

/// @brief Returns the maximum value of a or b
/// @param a 
/// @param b 
/// @return 
int max(int a, int b);

/// @brief Returns the minimum value of a or b
/// @param a 
/// @param b 
/// @return 
int min(int a, int b);

#endif