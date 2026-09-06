#include <init.h>
/*.
    THIS FILE IS JUST EXAMPLE! DO NOT USE IT WITHOUT MODIFICATIONS
*/

#define PBRV "../../Samples/SHADERS/pbr.vert"
#define PBRF "../../Samples/SHADERS/pbr.frag"
#define SKYBOXV "../../Samples/SHADERS/skybox.vert"
#define SKYBOXF "../../Samples/SHADERS/skybox.frag"
#define SHADOWV "../../Samples/SHADERS/shadow.vert"
#define SHADOWF "../../Samples/SHADERS/shadow.frag"

  void loop(){
GLuint PBR = prepshader(PBRV, PBRF);
GLuint skybox = prepshader(SKYBOXV, SKYBOXF);
GLuint Shadow = prepshader(SHADOWV, SHADOWF);

//model test = LoadMesh("../../Samples/DH_light/dh.gltf");
model test = LoadMesh("../../Samples/sponza/Untitled.gltf");

initshadow();

unsigned int hdr = Load_HDRenv("../../Samples/hdri.hdr");

while (!glfwWindowShouldClose(window)) {

        mat4 projection;
        //vec3 lightpos = {0.0f, 5.0f, 1.0f};
        mat4 view;
        vec3 center;

  //
  float currentframe = (float)glfwGetTime();
  deltatime = currentframe - lastframe;
  lastframe = currentframe;

  //
  // INPUT
  KeyCallback(window);
  CameraYawPitch();

  float halfHeight = 5.0f; // Controls how "zoomed in" you are
  float aspect = (float)WIDTH / (float)HEIGHT;
  float halfWidth = halfHeight * aspect;

  // CAMERA
  glm_vec3_add(localcamera.position, localcamera.front, center);
  glm_lookat(localcamera.position, center, localcamera.up, view);
  glm_perspective(glm_rad(45.0f), (float)WIDTH / (float)HEIGHT, 0.001f, 100.0f, projection);
  //glm_ortho(-halfWidth, halfWidth, -halfHeight, halfHeight, 0.001f, 100.0f, projection);
  //Calculate light matrix
  for(int i = 0; i < test.lightcount; i++){
      CalculateMatrixLight(test.lights[i].pos, test.lightspacematrix);
  }
  //

  //Model Matrix
  glm_mat4_identity(test.primitives->transform);
  GLenum err;
#ifdef NoRo_DEBUG_ON
  while ((err = glGetError()) != GL_NO_ERROR) {
      printf("ERR! = %d\n", err);
  }
#endif
  // SHADOW CALCULATION SECTION
  glViewport(0, 0, SHADOWWIDTH, SHADOWHEIGHT);
  glBindFramebuffer(GL_FRAMEBUFFER, shadowfbo);
  glClear(GL_DEPTH_BUFFER_BIT);
  glUseProgram(Shadow);

  DrawMesh(&test, Shadow, 0);

  // PBR
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  glViewport(0, 0, WIDTH,HEIGHT);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glUseProgram(PBR);


  glUniform3f((glGetUniformLocation(PBR, "u_CamPos")), localcamera.position[0],localcamera.position[1],localcamera.position[2]);

  glActiveTexture(GL_TEXTURE5);
  glBindTexture(GL_TEXTURE_2D, hdr);

  glUniform1i(glGetUniformLocation(PBR, "u_EnvMap"), 5);
  glUniform3f((glGetUniformLocation(PBR, "u_EmissiveFactor")), 1.0f,1.0f,1.0f);

  glUniformMatrix4fv(glGetUniformLocation(PBR, "view"), 1, GL_FALSE, (float*)view);
  glUniformMatrix4fv(glGetUniformLocation(PBR, "projection"), 1, GL_FALSE, (float*)projection);


  DrawMesh(&test, PBR, 0);

// Skybox

  glDepthMask(GL_FALSE);
  glDepthFunc(GL_LEQUAL);
  glUseProgram(skybox);

  glUniformMatrix4fv(glGetUniformLocation(skybox, "view"), 1, GL_FALSE, (float*)view);
  glUniformMatrix4fv(glGetUniformLocation(skybox, "projection"), 1, GL_FALSE, (float*)projection);

  DrawMesh(&test, skybox, hdr);
  glDepthFunc(GL_LESS);
  glDepthMask(GL_TRUE);

//

  glfwSwapBuffers(window);
  glfwPollEvents();
}

CleanupMesh(&test);

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
