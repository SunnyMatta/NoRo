#pragma once

#include <data.H>

float speed = 1.0f; // Camera movement speed

static inline void KeyCallback(GLFWwindow* window, float DELTA) {

    // Check for key presses and update camera position
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        glm_vec3_muladds(localcamera.front, speed * DELTA, localcamera.position); // Move forward
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        glm_vec3_muladds(localcamera.front, -speed * DELTA, localcamera.position);
    }

    vec3 right;
    glm_vec3_cross(localcamera.front, localcamera.up, right);
    glm_vec3_normalize(right);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        glm_vec3_muladds(right, -speed * DELTA, localcamera.position);
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        glm_vec3_muladds(right, speed * DELTA, localcamera.position);
    }

    // Check for arrow keys to adjust camera orientation
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
        localcamera.pitch += speed * 30 * DELTA; // Look up
    }
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
        localcamera.pitch -= speed * 30 * DELTA; // Look down
    }
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
        localcamera.yaw -= speed * 30 * DELTA; // Look left
    }
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
        localcamera.yaw += speed * 30 * DELTA; // Look right
    }
    }
