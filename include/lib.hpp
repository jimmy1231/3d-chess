#ifndef __RENDER_LIB_H__
#define __RENDER_LIB_H__

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/mat4x4.hpp>

void 
bind_tex(const unsigned char *data,
			   const int width,
			   const int height,
			   const GLuint textureUnit,
			   GLuint &TEX);

void 
bind_tex_fbo(const GLuint &TEX,
    			   GLuint &FBO);

void 
bind_vao(const std::vector<ld_o::VBO_STRUCT> &data,
         GLuint &VAO);

unsigned char *
load_tex(const std::string &filepath,
         int &width,
         int &height);

void 
bind_shaders(const std::vector<ShaderProg> &shader_progs,
             GLuint &prog_id);

void 
load_obj(std::string filepath
         std::vector<ld_o::VBO_STRUCT> &data);

namespace ld_o {
  typedef struct V { /* Vertices */ 
    float x;
    float y;
    float z;
    float w;
  } V;

  typedef struct VT { /* Textures */ 
    float u;
    float v;
    float w;
  } VT;

  typedef struct VN { /* Vertex Normals */
    float x;
    float y;
    float z;
  } VN;

  typedef struct VP { /* Free-form Geometry Statement */
    float u;
    float v;
    float w;
  } VP;

  typedef struct F { /* One vertex of a Face - polygon */
    int v;
    int vt;
    int vn;
  } F;
  
  typedef struct VBO_STRUCT {
    glm::vec3 v;
    glm::vec3 n;
    glm::vec2 t;
  } VBO_STRUCT;

  typedef std::vector<F *> Face;
}

namespace screen {
  enum class ImageType {
    IMAGE_TYPE_RGB,
    IMAGE_TYPE_GREYSCALE
  };

  void 
  screenshot(const GLuint FBO,
             const GLenum mode,
             const screen::ImageType type, 
             const int width,
             const int height,
             const char *output_file);
}

/* Make sure dimensions are the closest powers of 2 */
inline int clamp_dim(int dim) {
  int _dim = 1; /* 2^0 */
  while (true) {
    if (_dim > dim) {
      break;
    }
    _dim *= 2;
  }

  return _dim / 2;
}

#endif /* __RENDER_LIB_H__ */