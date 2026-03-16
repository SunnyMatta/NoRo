#pragma once

#include <data.h>
#include <render.h>

float speed = 1.0f; // Camera movement speed
float lastX = 0;
float lastY = 0;
bool firstmouse = true;
float sensitivity = 5.0f;

static inline void KeyCallback(GLFWwindow* window) {

    // Check for key presses and update camera position
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        glm_vec3_muladds(localcamera.front, speed * deltatime, localcamera.position); // Move forward
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        glm_vec3_muladds(localcamera.front, -speed * deltatime, localcamera.position);
    }

    vec3 right;
    glm_vec3_cross(localcamera.front, localcamera.up, right);
    glm_vec3_normalize(right);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        glm_vec3_muladds(right, -speed * deltatime, localcamera.position);
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        glm_vec3_muladds(right, speed * deltatime, localcamera.position);
    }

    // Check for arrow keys to adjust camera orientation
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
        localcamera.pitch += speed * 30 * deltatime; // Look up
    }
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
        localcamera.pitch -= speed * 30 * deltatime; // Look down
    }
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
        localcamera.yaw -= speed * 30 * deltatime; // Look left
    }
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
        localcamera.yaw += speed * 30 * deltatime; // Look right
    }
    }

void mousecallback(GLFWwindow* window, double xpos, double ypos){
    if (firstmouse) {
        lastX = xpos;
        lastY = ypos;
        firstmouse = false;
    }
    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;
    lastX = xpos;
    lastY = ypos;

    yoffset *= sensitivity * deltatime;
    xoffset *= sensitivity * deltatime;

    localcamera.yaw += xoffset;
    localcamera.pitch += yoffset;
}
