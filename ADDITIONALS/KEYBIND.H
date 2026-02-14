#pragma once

#include <data.H>

float speed = .1f; // Camera movement speed

static inline void KeyCallback(GLFWwindow* window) {

    // Check for key presses and update camera position
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        glm_vec3_muladds(localcamera.front, speed, localcamera.position); // Move forward
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        glm_vec3_muladds(localcamera.front, -speed, localcamera.position);
    }

    vec3 right;
    glm_vec3_cross(localcamera.front, localcamera.up, right);
    glm_vec3_normalize(right);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        glm_vec3_muladds(right, -speed, localcamera.position);
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        glm_vec3_muladds(right, speed, localcamera.position);
    }

    // Check for arrow keys to adjust camera orientation
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
        localcamera.pitch += speed; // Look up
    }
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
        localcamera.pitch -= speed; // Look down
    }
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
        localcamera.yaw -= speed; // Look left
    }
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
        localcamera.yaw += speed; // Look right
    }
    }
