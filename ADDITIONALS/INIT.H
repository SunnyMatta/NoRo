//
#pragma once
//

#include <RENDER.H>
#include <LOADMESH.H>
#include <cstdlib>

static inline int INIT(GLFWwindow** window) {
    if (!glfwInit()) {
        fprintf(stderr, "Failed to initialize Graphics Library\n");
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
    *window = glfwCreateWindow(WIDTH, HEIGHT, "OpenGL Window", NULL, NULL);
    if (!*window) {
        fprintf(stderr, "Failed to create window\n");
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(*window);
    if (glewInit() != GLEW_OK) {
        fprintf(stderr, "Failed to initialize GLEW\n");
        return -1;
    }

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glDisable(GL_CULL_FACE);
    return 0;
}


static inline void RENDER(GLFWwindow* window) {

        render(window);
}

static inline void CLEANUP(GLFWwindow* window) {
    glfwDestroyWindow(window);
    glfwTerminate();

}
