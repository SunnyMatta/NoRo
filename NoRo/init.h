//
#pragma once
//

#include <GLFW/glfw3.h>
#include <render.h>
#include <mesh.h>
#include <stdio.h>

static inline void CLEANUP(GLFWwindow* window);

static void Error_Callback(){
    fprintf(stderr, "Failed to Init GLFW, or something else happened :P")
}

static inline int INIT(GLFWwindow** window) {

    glfwSetErrorCallback(Error_Callback);
    if (!glfwInit()) {
        fprintf(stderr, "Failed to initialize Graphics Library\n");
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    #ifdef NoRo_TEST
    glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
    #endif

    *window = glfwCreateWindow(WIDTH, HEIGHT, "NoRo", NULL, NULL);
    if (!*window) {
        fprintf(stderr, "Failed to create window\n");
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(*window);

    if (glewInit() != GLEW_OK && !GLEW_ERROR_NO_GLX_DISPLAY) {
        fprintf(stderr, "Failed to initialize GLEW\n");
        return -1;
    }

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glDisable(GL_CULL_FACE);
    return 0;
}


static inline void RENDER(GLFWwindow* window, void (*external)(void)) {

        render(window, external);
}

static inline void CLEANUP(GLFWwindow* window) {
    glfwDestroyWindow(window);
    glfwTerminate();
}
