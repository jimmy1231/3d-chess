#ifndef __RENDER__BIND_TEX_FBO_H__
#define __RENDER__BIND_TEX_FBO_H__

#include <glad/glad.h>

void bind_tex_fbo(const GLuint &TEX, GLuint &FBO) {
  GLuint _FBO;
  
  glGenFramebuffers(1, &_FBO);
  glBindFramebuffer(GL_FRAMEBUFFER, _FBO);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
    GL_TEXTURE_2D, TEX, 0);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  FBO = _FBO;
}

#endif