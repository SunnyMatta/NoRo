#include <init.h>

/*.
I've decided to use headers for my entire project. I know it's weird approach.
*/

void setup(){

}

void loop(){

}

int main(void) {
    if (INIT(&window) != 0) {
        return -1;
    }
    RENDER(window,setup,loop);
    CLEANUP(window);
    return 0;
}
