#include <init.h>
/*.
    THIS FILE IS A TEST!
*/

#define PBRV "Tests/Shaders/pbr.vert"
#define PBRF "Tests/Shaders/pbr.frag"
#define SKYBOXV "Tests/Shaders/skybox.vert"
#define SKYBOXF "Tests/Shaders/skybox.frag"
#define SHADOWV "Tests/Shaders/shadow.vert"
#define SHADOWF "Tests/Shaders/shadow.frag"

  void loop(){

GLuint PBR = prepshader(PBRV, PBRF);
GLuint skybox = prepshader(SKYBOXV, SKYBOXF);
GLuint Shadow = prepshader(SHADOWV, SHADOWF);

model SponzaScene = LoadMesh("Tests/Model/Untitled.gltf");

unsigned int hdr = Load_HDRenv("Tests/hdr.hdr");

        mat4 projection;
        mat4 view;
        vec3 center;

  //Delta time Calc
  float currentframe = (float)glfwGetTime();
  deltatime = currentframe - lastframe;
  lastframe = currentframe;
  //

  // INPUT
  KeyCallback(window); // control for flying
  CameraYawPitch();

  float halfHeight = 5.0f; // Controls how "zoomed in" you are
  float aspect = (float)WIDTH / (float)HEIGHT;
  float halfWidth = halfHeight * aspect;

  // CAMERA
  glm_vec3_add(localcamera.position, localcamera.front, center);
  glm_lookat(localcamera.position, center, localcamera.up, view);
  glm_perspective(glm_rad(45.0f), (float)WIDTH / (float)HEIGHT, 0.001f, 100.0f, projection);

  // PBR
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  glViewport(0, 0, WIDTH,HEIGHT);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glUseProgram(PBR);

  glUniform3f((glGetUniformLocation(PBR, "u_CamPos")), localcamera.position[0],localcamera.position[1],localcamera.position[2]);
  glUniform3f((glGetUniformLocation(PBR, "u_EmissiveFactor")), 1.0f,1.0f,1.0f);

  glUniformMatrix4fv(glGetUniformLocation(PBR, "view"), 1, GL_FALSE, (float*)view);
  glUniformMatrix4fv(glGetUniformLocation(PBR, "projection"), 1, GL_FALSE, (float*)projection);

  glActiveTexture(GL_TEXTURE5);
  glBindTexture(GL_TEXTURE_2D, hdr);
  glUniform1i(glGetUniformLocation(PBR, "u_EnvMap"), 5);

  DrawMesh(&SponzaScene, PBR);

// Skybox
  glDepthMask(GL_FALSE);
  glDepthFunc(GL_LEQUAL);
  glUseProgram(skybox);

  glUniformMatrix4fv(glGetUniformLocation(skybox, "view"), 1, GL_FALSE, (float*)view);
  glUniformMatrix4fv(glGetUniformLocation(skybox, "projection"), 1, GL_FALSE, (float*)projection);

  glActiveTexture(GL_TEXTURE5);
  glBindTexture(GL_TEXTURE_2D, hdr);
  glUniform1i(glGetUniformLocation(skybox, "u_EnvMap"), 5);

  DrawMesh(&SponzaScene, skybox);
  glDepthFunc(GL_LESS);
  glDepthMask(GL_TRUE);
//

  glfwSwapBuffers(window);
  glfwPollEvents();


CleanupMesh(&SponzaScene);

glDeleteProgram(Shadow);
glDeleteProgram(PBR);
}

int main(void) {
    if (INIT(&window) != 0) {
        return -1;
    }
    RENDER(window,loop);
    CLEANUP(window);
    return 0;
}
