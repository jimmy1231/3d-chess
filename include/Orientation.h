#ifndef __RENDER_ORIENTATION_H__
#define __RENDER_ORIENTATION_H__

#include <glad/glad.h>
#include <glm/glm.hpp>
#include "helpers.h"

class Orientation {
  public:
    glm::mat4 view_;
    glm::mat4 per_;
    glm::mat4 scale_bias_;

    glm::vec3 eye;
    glm::vec3 gaze;
    glm::vec3 top;

    float fovy;
    float zNear;
    float zFar;

    Orientation() {}
    Orientation(std::string e, std::string g, std::string t,
                float f, float n, float a) 
      : eye(parse_vec3(e))
      , gaze(parse_vec3(g))
      , top(parse_vec3(t))
      , fovy(f)
      , zNear(n)
      , zFar(a)
    {
      gaze = glm::normalize(gaze);
    }

    const GLfloat *view() {
      // Compute the camera basis
      glm::vec3 w, u, v;
      w = -glm::normalize(gaze);
      u = glm::normalize(glm::cross(top, w));
      v = glm::cross(w, u);

      // M_cam = [ u v w e ]^-1, where u, v, w are
      // vec4 with the 4th index set to 0.
      view_ = glm::inverse(glm::mat4(
        glm::vec4(u,0),
        glm::vec4(v,0),
        glm::vec4(w,0),
        glm::vec4(eye,1)
      ));

      return (const GLfloat *)&view_;
    }

    const GLfloat *scale_bias() {
      scale_bias_ = glm::transpose(glm::mat4(
        0.5, 0.0, 0.0, 0.5,
        0.0, 0.5, 0.0, 0.5,
        0.0, 0.0, 0.5, 0.5,
        0.0, 0.0, 0.0, 1.0
      ));

      return (const GLfloat *)&scale_bias_;
    }

    const GLfloat *perspective(const float &width, const float &height) {
      per_ = glm::perspective(
        glm::radians(fovy), width/height,
        zNear, zFar  
      );
      return (const GLfloat *)&per_;
    }
};

#endif /* __RENDER_ORIENTATION_H__ */