#ifndef __RENDER_MODEL_H__
#define __RENDER_MODEL_H__

#include <glad/glad.h>
#include <glm/glm.hpp>

#include "helpers.h"
#include "Texture.h"

class Model {
  public:
    glm::mat4 model_; 

    Texture *tex_;
    Data *data_;

    float rotationDeg_;
    glm::vec3 rotationAxis_;
    glm::vec3 scale_;
    glm::vec3 translate_;

    Model(Data *d, Texture *t, float r, 
          std::string a, std::string s, std::string t)
      : data_(d)
      , tex_(t)
      , rotationDeg_(r)
      , rotationAxis_(parse_vec3(a))
      , scale_(parse_vec3(s))
      , translate_(parse_vec3(t)) {}

    const GLfloat *model() {
      glm::mat4 rot = gcc_test::rot_about(
        glm::radians(rotationDeg_),
        rotationAxis_);
      glm::mat4 scale = glm::transpose(glm::mat4(
        scale_[0], 0.0f, 0.0f, 0.0f,
        0.0f, scale_[1], 0.0f, 0.0f,
        0.0f, 0.0f, scale_[2], 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
      ));
      glm::mat4 trans = glm::transpose(glm::mat4(
        1.0f, 0.0f, 0.0f, translate_[0],
        0.0f, 1.0f, 0.0f, translate_[1],
        0.0f, 0.0f, 1.0f, translate_[2],
        0.0f, 0.0f, 0.0f, 1.0f
      ));
      model_ = rot * scale * trans;
      return (const GLfloat *)&model_;       
    }
};

#endif /* __RENDER_MODEL_H__ */