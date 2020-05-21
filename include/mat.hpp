#ifndef __RENDER_MAT_H__
#define __RENDER_MAT_H__

#include <glm/glm.hpp>
#include <glm/mat4x4.hpp>

namespace mat {
  extern glm::mat4 shadow_bias;

  glm::mat4
  mat::perspective(const int width,
                             const int height,
                             const float n,
                             const float f,
                             const float fov_deg);

  glm::mat4 
  mat::view(const glm::vec3 &g,
                      const glm::vec3 &t,
                      const glm::vec3 &e);

  glm::mat3 
  mat::rot(const float &theta,
                     const glm::vec3 &a);
}

#endif /* __RENDER_MAT_H__ */