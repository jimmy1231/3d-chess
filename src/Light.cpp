#include <glm/glm.hpp>

#include "types.hpp"
#include "mat.hpp"
#include "helpers.h"


Light::Light(std::string p, std::string i) 
  : position(parse_vec3(p))
  , intensity(parse_vec3(i)) {}

void Light::initShadow(const GLuint prog, Scene &scene) {
  shadow = new Texture(NULL, scene.WIDTH, scene.HEIGHT);

  glm::vec3 g, t, e;
  g = glm::vec3(0,0,0) - position;
  t = scene.orient->top;
  e = position;

  view = mat::view(g, t, e);
	per = mat::perspective(scene.WIDTH, scene.HEIGHT,
                         1.0f, 100.0f,
                         30.0f);

  shadow->shadow_map(prog, scene, per, view);
}

glm::mat4 Light::shadowMat() {
  glm::mat4 bias{
    0.5, 0.0, 0.0, 0.0,
    0.0, 0.5, 0.0, 0.0,
    0.0, 0.0, 0.5, 0.0,
    0.5, 0.5, 0.5, 1.0
  };
  return bias * view * per; 
}
