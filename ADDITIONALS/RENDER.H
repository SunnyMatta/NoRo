#pragma once
//

#include <KEYBIND.H>
#include <data.H>
#include <LOADMESH.H>
#include <perlin.h>

#define _1VERT "/home/mint/Documents/Projects/DATA/SHADERS/shader.vert"
#define _1FRAQ "/home/mint/Documents/Projects/DATA/SHADERS/shader.frag"

Camera localcamera = {{0.0f, 0.0f, 5.0f}, 0.0f, -90.0f, {0.0f, 0.0f, -1.0f}, {0.0f, 1.0f, 0.0f}}; // Initial camera position and orientation

float deltatime = 0;

static inline void ProjectionSetup(GLFWwindow* window, int width, int height);
static inline void updatecameravectors();
static inline void render(GLFWwindow* window);

static inline void ProjectionSetup(GLFWwindow* window, int width, int height, GLuint Program) {
    glfwGetFramebufferSize(window, &width, &height);
    glViewport(0, 0, width, height);
    mat4 projection;
    
    glm_perspective(glm_rad(45.0f), (float)width / (float)height, 0.1f, 100.0f, projection);
    glUseProgram(Program);
    GLint projectionlocation = glGetUniformLocation(Program, "projection");
    glUniformMatrix4fv(projectionlocation, 1, GL_FALSE, (float*)projection);
}

static inline void updatecameravectors() {

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
    glShaderSource(vs,1,&vertex,NULL);
    glCompileShader(vs);
    glGetShaderiv(vs, GL_COMPILE_STATUS, &success);

    if (!success) {
        glGetShaderInfoLog(vs,512,NULL,infolog);
        printf("Vertex Shader didn't compiled\n%s\n",infolog);
        
    }

    GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fs,1,&fragment,NULL);
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

static inline void render(GLFWwindow* window) {

        GLuint program = prepshader(_1VERT, _1FRAQ);

        model test = LoadMesh("DATA/DamagedHelmet.glb");
        if (test.count == 0) {
            printf("err");
            return;
        }

        ProjectionSetup(window, WIDTH, HEIGHT, program);

        while (!glfwWindowShouldClose(window)) {
        float currentframe = (float)glfwGetTime();
        deltatime = currentframe - lastframe;
        lastframe = currentframe;
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f); 
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(program);

        GLint lightposloc = glGetUniformLocation(program, "lightPos");
        glUniform3f(lightposloc, 1.2f,1.0f,2.0f);

        GLint lightcolorloc = glGetUniformLocation(program, "lightColor");
        glUniform3f(lightcolorloc, 1.0f,2.0f,1.0f);

        GLint objcolorloc = glGetUniformLocation(program, "objectColor");
        glUniform3f(objcolorloc, 1.0f,0.5f,1.0f);

        GLint viewposloc = glGetUniformLocation(program, "viewPos");
        glUniform3f(viewposloc, localcamera.position[0],localcamera.position[1],localcamera.position[2]);
        // Render here
        glLoadIdentity();
        KeyCallback(window);
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        glfwSetCursorPosCallback(window, mousecallback);
        //transform camera position and rotation based on localcamera position and orientation
        mat4 view;
        vec3 center;

        updatecameravectors();
        
        DrawMesh(&test);
        
        glm_vec4_add(localcamera.position, localcamera.front, center);
        glm_lookat(localcamera.position, center, localcamera.up, view);
        GLint viewloc = glGetUniformLocation(program, "view");
        glUniformMatrix4fv(viewloc, 1, GL_FALSE, (float*)view);

        mat4 modelpos;

        vec3 modelposition = {0,0,2};
        glm_mat4_identity(modelpos);
        glm_translate(modelpos, modelposition);
        glm_scale_uni(modelpos, 0.1f);

        GLint modelloc = glGetUniformLocation(program, "model");
        glUniformMatrix4fv(modelloc, 1, GL_FALSE, (float*)modelpos);

        glfwSwapBuffers(window);
        glfwPollEvents();
        }

        glDeleteProgram(program);

        CleanupMesh(&test);

}
