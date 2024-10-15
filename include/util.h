// this file include utility functions that may be useful
#include <definitions.h>

/// @brief println works exactly like printf except it adds a newline
/// This should avoid situations where you forget to print a newline.
/// @param format 
/// @param  
extern void println(const char *format, ...);

/// @brief Clears the terminal screen, will adapt to unix or windows.
void clearScreen();

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