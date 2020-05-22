#include <glm/gtc/type_ptr.hpp>
#include <glad/glad.h>
#include "lib.hpp"
#include "types.hpp"

#define SCENE_DEBUG 0

GLenum ShadowMap::DRAW_BUFFERS[1] = {GL_DEPTH_ATTACHMENT};

ShadowMap::ShadowMap(std::vector<Light> &lights,
                     std::vector<Model *> &models,
                     int width,
                     int height,
					           std::string nvs,
                     std::string nfs) 
{
  glGenTextures(1, &tex);
  glBindTexture(GL_TEXTURE_2D_ARRAY, tex);
  glTexStorage3D(GL_TEXTURE_2D_ARRAY,
                1,
                GL_DEPTH_COMPONENT32, 
                width, 
                height, 
                lights.size());

  glTexParameteri(GL_TEXTURE_2D_ARRAY,
                  GL_TEXTURE_WRAP_T,
                  GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D_ARRAY, 
                  GL_TEXTURE_WRAP_S, 
                  GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D_ARRAY, 
                  GL_TEXTURE_MIN_FILTER, 
                  GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D_ARRAY, 
                  GL_TEXTURE_MAG_FILTER, 
                  GL_LINEAR);

  /* 
   * Set up a texture evaluation function:
   * REF (r): current interpolated texture coordinate.
   * TEXTURE (Dt): depth texture value sampled from the 
   *               currently bound depth texture.
   * Says:
   * Every fragment 'r' that has a depth value <= the
   * the bound depth texture will have a value of 1.0,
   *
   * Formally:
   *    result = r <= Dt ? 1.0 : 0
   */
  glTexParameteri(GL_TEXTURE_2D_ARRAY, 
                  GL_TEXTURE_COMPARE_MODE,
                  GL_COMPARE_REF_TO_TEXTURE);
  glTexParameteri(GL_TEXTURE_2D_ARRAY, 
                  GL_TEXTURE_COMPARE_FUNC, 
                  GL_LEQUAL);

  glBindTexture(GL_TEXTURE_2D_ARRAY, 0);

  GLuint prog = load_shaders_simple(nvs, nfs); 
  glGenFramebuffers(1, &fbo);
  glBindFramebuffer(GL_FRAMEBUFFER, fbo);

  /* Iniitalize program outside */
  for (int i=0; i<lights.size(); ++i) {
    Light &light = lights[i];

    glUseProgram(prog);
    glFramebufferTextureLayer(GL_FRAMEBUFFER,
                              GL_DEPTH_ATTACHMENT,
                              tex,
                              0,
                              i);
    glDrawBuffers(1, &DRAW_BUFFERS[0]);

    glClearColor(1.0f,1.0f,1.0f,1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_FRONT);

    glUniformMatrix4fv(0, 1, false, glm::value_ptr(light.per));
    glUniformMatrix4fv(1, 1, false, glm::value_ptr(light.view));
    for (Model *&model : models) {
      glUniformMatrix4fv(2, 1, false, model->model());
      glBindVertexArray(model->data_->vao);
      glDrawArrays(GL_TRIANGLES, 0, model->data_->size());

      glBindVertexArray(0);
    }

    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
    glUseProgram(0);

#if SCENE_DEBUG
    char screenshot_filename[30];
    snprintf(screenshot_filename, 30, "../shadow_map_%d.tga", i);

    screen::screenshot(fbo,
       GL_DEPTH_ATTACHMENT,
       screen::ImageType::IMAGE_TYPE_GREYSCALE,
       width, height,
       screenshot_filename);
#endif
  }

  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}