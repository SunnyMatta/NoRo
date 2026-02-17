#pragma once
//

#include <KEYBIND.H>
#include <sys/types.h>
#include <data.H>
#include <LOADMESH.H>
#include <perlin.h>
#include <USRDATA.H>

#define _1VERT "/home/mint/Documents/Projects/DATA/SHADERS/shader.vert"
#define _1FRAQ "/home/mint/Documents/Projects/DATA/SHADERS/shader.frag"

Camera localcamera = {{0.0f, 0.0f, 5.0f}, 0.0f, -90.0f, {0.0f, 0.0f, -1.0f}, {0.0f, 1.0f, 0.0f}}; // Initial camera position and orientation

float deltatime = 0;

inline void ProjectionSetup(GLFWwindow* window, int width, int height);
inline void CameraYawPitch();
static inline void render(GLFWwindow* window);


inline void ProjectionSetup(GLFWwindow* window, int width, int height, GLuint Program) {
    glfwGetFramebufferSize(window, &width, &height);
    glViewport(0, 0, width, height);
    mat4 projection;
    
    glm_perspective(glm_rad(45.0f), (float)width / (float)height, 0.1f, 100.0f, projection);
    glUseProgram(Program);
    GLint projectionlocation = glGetUniformLocation(Program, "projection");
    glUniformMatrix4fv(projectionlocation, 1, GL_FALSE, (float*)projection);
}

inline void CameraYawPitch() {

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

        model test[2] = {LoadMesh("DATA/DamagedHelmet.glb")};

        if (test[0].count == 0) {
            printf("err");
            return;
        }

        Vertex* vertex = NULL;
        uint32_t* indic = NULL;


        int imountain = flatrandom(&vertex, &indic, 200);
        int vmountain = 20*20;

        model mountain = InitMesh(vertex, vmountain, indic, imountain);
        

        free(vertex);
        free(indic);

        ProjectionSetup(window, WIDTH, HEIGHT, program);

        while (!glfwWindowShouldClose(window)) {
        //
        float currentframe = (float)glfwGetTime();
        deltatime = currentframe - lastframe;
        lastframe = currentframe;
        // 

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f); 
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(program); //shader


        GLint flatcolloc = glGetUniformLocation(program, "flatcolor");
        glUniform3f(flatcolloc, 0.5f,0.5f,0.5f);

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


        CameraYawPitch();

        // CAMERA
        mat4 view;
        vec3 center;
        glm_vec4_add(localcamera.position, localcamera.front, center);
        glm_lookat(localcamera.position, center, localcamera.up, view);
        GLint viewloc = glGetUniformLocation(program, "view");
        glUniformMatrix4fv(viewloc, 1, GL_FALSE, (float*)view);

        //
        //

        mat4 helmet_m;
        GLint helmet_u = glGetUniformLocation(program, "model");
        vec3 helmet_p= {0,0,0};
        MeshOperator4fv(program, helmet_u, helmet_m, helmet_p, 1.0f);
        DrawMeshLOD(test, (float*)view, helmet_p);


        glfwSwapBuffers(window);
        glfwPollEvents();
        }

        glDeleteProgram(program);
        CleanupMesh(&test[0]);
        CleanupMesh(&mountain);

}
