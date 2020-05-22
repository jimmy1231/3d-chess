#include <iostream>
#include <glad/glad.h>
#include <string>

#include "types.hpp"
#include "mat.hpp"
#include "lib.hpp"


Texture::Texture(std::string f) : file(f) {
  glGenTextures(1, &id);
  data = load_tex(file, width, height);
  glBindTexture(GL_TEXTURE_2D, id);

  glTexParameteri(GL_TEXTURE_2D, 
                  GL_TEXTURE_WRAP_S, 
                  GL_CLAMP);
  glTexParameteri(GL_TEXTURE_2D, 
                  GL_TEXTURE_WRAP_T, 
                  GL_CLAMP);
  glTexParameteri(GL_TEXTURE_2D, 
                  GL_TEXTURE_MAG_FILTER, 
                  GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, 
                  GL_TEXTURE_MIN_FILTER, 
                  GL_LINEAR);

  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 
    height, width, 0, 
    GL_RGB, GL_UNSIGNED_BYTE, data);

  glActiveTexture(0);
  glBindTexture(GL_TEXTURE_2D, 0);
}


Texture::Texture() {}

Texture::~Texture() {
  if (data != NULL) {
    free(data);
  }
  glDeleteTextures(1, &id);
}
