#include <glm/glm.hpp>

#include "types.hpp"
#include "mat.hpp"
#include "helpers.h"


Light::Light(std::string p, std::string i, glm::vec3 t, int w, int h) 
  : position(parse_vec3(p))
  , intensity(parse_vec3(i)) 
{
  glm::vec3 g, e;
  g = glm::vec3(0,0,0) - position;
  e = position;

  view = mat::view(g, t, e);
  per = mat::perspective(w, h, 1.0f, 100.0f, 30.0f);
}

glm::mat4 Light::Mvp_bias() {
  return mat::shadow_bias * per * view;
}

glm::mat4 Light::Mvp() {
  return per * view;
}