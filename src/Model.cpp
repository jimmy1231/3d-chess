#include <glad/glad.h>
#include <glm/glm.hpp>

#include "mat.hpp"
#include "types.hpp"
#include "helpers.h"


Model::Model(Data *d, Texture *t, float r, 
  std::string a, std::string s, std::string tr)
	: data_(d)
	, tex_(t)
	, rotationDeg_(r)
	, rotationAxis_(parse_vec3(a))
	, scale_(parse_vec3(s))
	, translate_(parse_vec3(tr)) {}

const GLfloat *Model::model() {
  glm::mat4 rot, scale, trans;

  rot = mat::rot(glm::radians(rotationDeg_), rotationAxis_);

  scale = glm::transpose(glm::mat4(
    scale_[0], 0.0f, 0.0f, 0.0f,
    0.0f, scale_[1], 0.0f, 0.0f,
    0.0f, 0.0f, scale_[2], 0.0f,
    0.0f, 0.0f, 0.0f, 1.0f
  ));

  trans = glm::transpose(glm::mat4(
    1.0f, 0.0f, 0.0f, translate_[0],
    0.0f, 1.0f, 0.0f, translate_[1],
    0.0f, 0.0f, 1.0f, translate_[2],
    0.0f, 0.0f, 0.0f, 1.0f
  ));

  model_ = rot * scale * trans;
  return (const GLfloat *)&model_;       
}
