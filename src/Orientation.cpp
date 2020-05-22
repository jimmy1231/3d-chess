#include <glad/glad.h>
#include <glm/glm.hpp>
#include "types.hpp"
#include "helpers.h"

Orientation::Orientation() {}

Orientation::Orientation(std::string e, std::string g, std::string t,
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

const GLfloat *Orientation::view() {
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

const GLfloat *Orientation::perspective(const float &width, const float &height) {
  per_ = glm::perspective(
    glm::radians(fovy), width/height,
    zNear, zFar  
  );
  return (const GLfloat *)&per_;
}
