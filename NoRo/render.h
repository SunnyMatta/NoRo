#pragma once
//
#include <keybind.h>
#include <data.h>
#include <mesh.h>
#include <userdata.h>

Camera localcamera = {{0.0f, 0.0f, 5.0f}, 0.0f, -90.0f, {0.0f, 0.0f, -1.0f}, {0.0f, 1.0f, 0.0f}}; // Initial camera position and orientation

float deltatime = 0;

void CalculateMatrixLight(vec3 lightposition, mat4 destination){
    mat4 lightprojection, lightview;

    glm_ortho(-10.0f, 10.0f, -10.0f, 10.0f, 0.1f, 40.0f, lightprojection);
    vec3 center = {0.0f, 0.0f, 0.0f};
    vec3 up = {0.0f, 1.0f, 0.0f};
    glm_lookat(lightposition,center,up,lightview);

    glm_mat4_mul(lightprojection, lightview, destination);
}


void ProjectionSetup(GLFWwindow* window, int width, int height, GLuint Program) {
    glfwGetFramebufferSize(window, &width, &height);
    glViewport(0, 0, width, height);
    mat4 projection;
    
    glm_perspective(glm_rad(45.0f), (float)width / (float)height, 0.1f, 100.0f, projection);
    glUseProgram(Program);
    GLint projectionlocation = glGetUniformLocation(Program, "projection");
    glUniformMatrix4fv(projectionlocation, 1, GL_FALSE, (float*)projection);
}

void CameraYawPitch() {
    vec3 front;
    front[0] = cos(glm_rad(localcamera.yaw)) * cos(glm_rad(localcamera.pitch));
    front[1] = sin(glm_rad(localcamera.pitch));
    front[2] = sin(glm_rad(localcamera.yaw)) * cos(glm_rad(localcamera.pitch));
    glm_vec3_normalize(front);
    glm_vec3_copy(front, localcamera.front);
}

GLuint prepshader(const char* vert, const char* frag){
    char* vertex = filetostring(vert);
    char* fragment = filetostring(frag);
    char infolog[512];
    int success;
    GLuint vs = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vs,1,(const GLchar * const*)&vertex,NULL);
    glCompileShader(vs);
    glGetShaderiv(vs, GL_COMPILE_STATUS, &success);

    if (!success) {
        glGetShaderInfoLog(vs,512,NULL,infolog);
        printf("Vertex Shader didn't compiled\n%s\n",infolog);
        
    }

    GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fs,1,(const GLchar * const*)&fragment,NULL);
    glCompileShader(fs);
    glGetShaderiv(fs, GL_COMPILE_STATUS, &success);

    if (!success) {
        glGetShaderInfoLog(fs,512,NULL,infolog);
        printf("Fragment Shader didn't compiled\n%s\n",infolog);
        
    }

    GLuint prog = glCreateProgram();
    glAttachShader(prog,vs);
    glAttachShader(prog, fs);
    glLinkProgram(prog);
    glGetShaderiv(prog, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(prog, 512, NULL, infolog);
        printf("Shader Program Didn't created\n%s\n", infolog);
    }

    glDeleteShader(vs);
    glDeleteShader(fs);
    free(fragment);
    free(vertex);

    return prog;
} 

float lastframe = 0.0f;

static void render(GLFWwindow* window, void (*External)(void)) {
        External();
}
