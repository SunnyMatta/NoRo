#include <init.h>

/*.
I've decided to use headers for my entire project. I know it's weird approach.
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