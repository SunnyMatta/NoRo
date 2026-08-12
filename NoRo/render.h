#pragma once
//
#include <keybind.h>
#include <data.h>
#include <mesh.h>
#include <userdata.h>

#define _1VERT "../../Samples/SHADERS/pbr.vert"
#define _1FRAQ "../../Samples/SHADERS/pbr.frag"
#define _2VERT "../../Samples/SHADERS/shadow.vert"
#define _2FRAQ "../../Samples/SHADERS/shadow.frag"

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

static void render(GLFWwindow* window, void (*External)(void), void (*Externalloop)(void)) {

        GLuint PBR = prepshader(_1VERT, _1FRAQ);
        GLuint Shadow = prepshader(_2VERT, _2FRAQ);
        External();
    
        //model test = LoadMesh("../../data/DH_light/dh.gltf");
        model test = LoadMesh("../../Samples/sponza/Untitled.gltf");
        
        initshadow();

        while (!glfwWindowShouldClose(window)) {

                mat4 lightspacematrix;
                mat4 projection;
                //vec3 lightpos = {0.0f, 5.0f, 1.0f};
                mat4 view;
                vec3 center;

        //
        float currentframe = (float)glfwGetTime();
        deltatime = currentframe - lastframe;
        lastframe = currentframe;
        Externalloop();
        // 
        // INPUT
        KeyCallback(window);
        CameraYawPitch();

        // CAMERA
        glm_vec3_add(localcamera.position, localcamera.front, center);
        glm_lookat(localcamera.position, center, localcamera.up, view);
        glm_perspective(glm_rad(45.0f), (float)WIDTH / (float)HEIGHT, 0.1f, 100.0f, projection);
        
        //Calculate light matrix
        //for(int i; i < test.lightcount; i++){
        //    CalculateMatrixLight(test.lights[i].pos, lightspacematrix);
        //}
        //

        //Model Matrix
        //glm_mat4_identity(test.primitives->transform);
        GLenum err;

        while ((err = glGetError()) != GL_NO_ERROR) {
            printf("ERR! = %d\n", err);
        }
        // SHADOW CALCULATION SECTION
        glViewport(0, 0, SHADOWWIDTH, SHADOWHEIGHT);
        glBindFramebuffer(GL_FRAMEBUFFER, shadowfbo);
        glClear(GL_DEPTH_BUFFER_BIT);
        glUseProgram(Shadow);

        glUniformMatrix4fv(glGetUniformLocation(Shadow, "u_LightSpaceMatrix"), 1, GL_FALSE, (float*)lightspacematrix);
        glUniformMatrix4fv(glGetUniformLocation(Shadow, "model"), 1, GL_FALSE, (float*)test.primitives->transform);
        
        
        DrawMesh(&test, Shadow);
        
        // PBR
        glBindFramebuffer(GL_FRAMEBUFFER, 0); 
        glViewport(0, 0, WIDTH,HEIGHT);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(PBR);
        
        //glUniform3f((glGetUniformLocation(PBR, "u_LightPos")), lightpos[0],lightpos[1],lightpos[2]);

        glUniform3f((glGetUniformLocation(PBR, "u_LightColor")), 10.0f,10.0f,10.0f);

        glUniform3f((glGetUniformLocation(PBR, "u_CamPos")), localcamera.position[0],localcamera.position[1],localcamera.position[2]);
        
        glUniform3f((glGetUniformLocation(PBR, "u_EmissiveFactor")), 1.0f,1.0f,1.0f);
        
        glUniformMatrix4fv(glGetUniformLocation(PBR, "view"), 1, GL_FALSE, (float*)view);
        glUniformMatrix4fv(glGetUniformLocation(PBR, "projection"), 1, GL_FALSE, (float*)projection);
        glUniformMatrix4fv(glGetUniformLocation(PBR, "model"), 1, GL_FALSE, (float*)test.primitives->transform);
        glUniformMatrix4fv(glGetUniformLocation(PBR, "u_LightSpaceMatrix"), 1, GL_FALSE, (float*)lightspacematrix);
        

        DrawMesh(&test, PBR);

        //
        glfwSwapBuffers(window);
        glfwPollEvents();
        }

        glDeleteProgram(Shadow);
        glDeleteProgram(PBR);
        CleanupMesh(&test);

}
