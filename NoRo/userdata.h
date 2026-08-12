#pragma once

#include "perlin.h"
#include "math.h"
#include <stdlib.h>
#include <strings.h>

typedef struct {
    float x, y, z;    // Location 0
    float nx, ny, nz; // Location 1 (Normals)
    float u, v;       // Location 2 (TexCoords)
} Vertex;


int flatrandom(Vertex **vertices, unsigned int** indices, int grid_size){
    int num_vert =grid_size * grid_size;
    int num_indic = (grid_size - 1) * (grid_size - 1) * 6;

    *vertices = (Vertex*)malloc(sizeof(Vertex) * num_vert);
    *indices = (unsigned int*)malloc(sizeof(Vertex) * num_indic);

    float spacing = 0.1f;
    float amplitute = 10.0f;


    
    for (int z = 0; z < grid_size; z++) {
        for (int x = 0; x < grid_size; x++) {
            int index = z * grid_size + x;

            float xpos = (float)x * spacing;
            float zpos = (float)x * spacing;


            float noisevalue = (float)pnoise2d(xpos, zpos, 0.5, 6, 1);

            //sharp
            noisevalue = powf(noisevalue, 2.0f);

            (*vertices)[index].x = xpos;
            (*vertices)[index].y = noisevalue * amplitute;
            (*vertices)[index].z = zpos;

            (*vertices)[index].nx = 0.0f;
            (*vertices)[index].ny = 1.0f;
            (*vertices)[index].nz = 0.0f;

            (*vertices)[index].u = 0.0f;
            (*vertices)[index].v = 0.0f;
        }
    }

    int current = 0;
    for (int z = 0;z < grid_size; z++) {
        for (int x = 0;x < grid_size; x++) {
            int row1 = z * grid_size;
            int row2 = (z + 1) * grid_size;

            (*indices)[current++] = row1 + x;
            (*indices)[current++] = row1 + x + 1;
            (*indices)[current++] = row2 + x;

            (*indices)[current++] = row1 + x + 1;
            (*indices)[current++] = row2 + x + 1;
            (*indices)[current++] = row2 + x;
        }
    }
        
    return num_indic;
}

