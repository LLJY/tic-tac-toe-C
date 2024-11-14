#include <util.h>

inline void println(const char *format, ...) {
  va_list args;
  va_start(args, format);
  vprintf(format, args);
  va_end(args);
  printf("\n"); 
}

void clearScreen() {
    #ifdef _WIN32
        system("cls"); 
    #else
        system("clear"); 
    #endif
}

void clearInputBuffer(){
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

/// @brief Returns the maximum value of a or b
/// @param a 
/// @param b 
/// @return 
int max(int a, int b) {
    return (a > b) ? a : b;
}

/// @brief Returns the minimum value of a or b
/// @param a 
/// @param b 
/// @return 
int min(int a, int b) {
    return (a < b) ? a : b;
}