#include <init.h>

/*
This is the main file (no shit).
I decided to use headers for my entire project, thus this file is apparatus for compilation.
*/

GLFWwindow* window = NULL;

int main(void) {
    if (INIT(&window) != 0) {
        return -1;
    }
    RENDER(window);
    CLEANUP(window);
    return 0;
}