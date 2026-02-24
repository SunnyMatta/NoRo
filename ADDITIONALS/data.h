#pragma once

/*
2026-02-13
data.H file contains all the necessary includes for the project.

It also can contain global variables that are shared across multiple headers/sources.

*/

#include <GL/glew.h>

#include <cglm/cglm.h>
#include <GLFW/glfw3.h>

#include <stdlib.h>
#include <string.h>

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
