#pragma once

#include "perlin.h"
#include <sys/types.h>
void dostuff(){
    int x = 0, y = 0;


    char elevation[y][x] = noise2d(x, y, 10, 1);
}