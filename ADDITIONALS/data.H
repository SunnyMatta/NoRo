#pragma once

/*
2026-02-13
data.H file contains all the necessary includes for the project.

It also can contain global variables that are shared across multiple headers/sources.

*/

#include "cglm/types.h"
#include <GL/glew.h>

#include <cglm/cglm.h>
#include <GLFW/glfw3.h>

#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

# define HEIGHT 600 // Default window height
# define WIDTH 800 // Default window width

// Camera structure to hold position and orientation
typedef struct {
    vec3 position; // Position
    double pitch, yaw; // Orientation
    vec3 front; // Camera front vector
    vec4 up; // Camera up vector
} Camera;

typedef struct {
    mat4 Model;
    vec3 Position;
    float Scale;
}Object;
//

GLuint loaddds(const char* file){
    unsigned char header[124];
    FILE *fd = fopen(file, "rb");
    if (fd == NULL) {
        return 0;
    }

    char filecode[4];
    fread(filecode, 1, 4, fd);
    if(strncmp(filecode, "DDS ", 4) != 0){
        fclose(fd);
        return 0;
    }

    fread(&header, 124, 1, fd);

    uint32_t height        = *(uint32_t*)&(header[8]);
    uint32_t width         = *(uint32_t*)&(header[12]);
    uint32_t linearsize    = *(uint32_t*)&(header[16]);
    uint32_t mipmapcount   = *(uint32_t*)&(header[24]);
    uint32_t fourCC        = *(uint32_t*)&(header[80]);

    unsigned char * buffer;
    uint32_t bufsize = mipmapcount > 1 ? linearsize * 2 : linearsize;
    buffer = (unsigned char*)malloc(bufsize*sizeof(unsigned char));

    fread(buffer, 1, bufsize, fd);
    fclose(fd);

    uint32_t format;
    switch (fourCC) {
        case 0x31545844: format = GL_COMPRESSED_RGBA_S3TC_DXT1_EXT; break;
        case 0x33545844: format = GL_COMPRESSED_RGBA_S3TC_DXT3_EXT; break;
        case 0x35545844: format = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT; break;
        default: free(buffer); return 0;
    }

    GLuint textureid;
    glGenTextures(1, &textureid);
    glBindTexture(GL_TEXTURE_2D, textureid);
    uint32_t blocksize = (format ==  GL_COMPRESSED_RGBA_S3TC_DXT1_EXT) ?  8 : 16;
    uint32_t offset = 0;

    for (uint32_t k = 0; k < mipmapcount && (width || height); ++k) {
        uint32_t size = ((width+3)/4) * ((height * 3) / 4) * blocksize;
        glCompressedTexImage2D(GL_IMAGE_2D, k ,format, width, height, 0, size, buffer + offset);
        offset += size;
        width /= 2;
        height /= 2;
    }
    free(buffer);
    return textureid;

}

char* filetostring(const char* filepath) {
    FILE* file = fopen(filepath, "rb");
    if (!file) {
        fprintf(stderr, "Failed to open shader file: %s\n", filepath);
        return NULL;
    }

    fseek(file, 0, SEEK_END);
    long length = ftell(file);
    fseek(file, 0, SEEK_SET);

    char* buffer = (char*)malloc(length + 1);
    if (buffer) {
        fread(buffer, 1, length, file);
        buffer[length] = '\0'; // Null-terminate the string
    }
    fclose(file);
    return buffer;
}

// Global variables
extern Camera localcamera;
extern GLFWwindow* window;
extern float deltatime;
//
