#include <glm/glm.hpp>
#include <glm/mat4x4.hpp>
#include <glm/geometric.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <stdio.h>
#include <fstream>
#include <iostream>
#include <cmath>

#include "helpers.h"
#include "mat.hpp"
#include "lib.hpp"

#define _DEBUG_MAT_LOGS_ 0 


glm::mat4 mat::shadow_bias{
  0.5, 0.0, 0.0, 0.0,
  0.0, 0.5, 0.0, 0.0,
  0.0, 0.0, 0.5, 0.0,
  0.5, 0.5, 0.5, 1.0
};

/*
 * Matrix that transforms coordinates from camera
 * space to the canonical view volume [-1,1]^3
 * As well, it applies the perspective transformation
 * using "near" and "far" as parameters.
 *
 * top, bot, left, right, near, far:
 *  Bounding planes for the orthogonal view volume
 *
 * Note: All inputs are presented in camera coordinates.
 *  i.e. y=3 --> y is the referential y-axis in the camera
 *  coordinate system.
 *
 * Inputs:
 *  GLfloat &top : y=top plane 
 */
glm::mat4 mat::perspective(const int width,
                           const int height,
                           const float n, const float f,
                           const float fov_deg)
{
  /*
   * Takes care of clipping as well as customized FOV.
   *
   * Param 1: FOV (field of view)
   * Param 2: Aspect ratio of screen (for clipping)
   * Param 3: near
   * Param 4: far
   *
   * Try an experiment:
   * Set far to 10.0f, and try to zoom out, you'll 
   * notice that at some point, the objects disappear.
   * This is because they have receded from the 
   * boundaries of the Orthogonal View Volume (which
   * is bound by the z=near, z=far planes)
   *
   * A good analogy to the Orthogonal View Volume is
   * your Field of View. Near is as close as you can
   * see (i.e. right up to your eye), far is as far
   * as you can see (i.e. the horizon, wherever that
   * may be), left and right are the limits to your
   * peripheral vision.
   * 
   * Note: notice that glm::perspective does not ask
   * for the "left" or "right" parameters which complete
   * the Orthogonal View Volume. This is because left
   * and right can be derived from the aspect ratio
   * of the screen!
   */
  float fovy, aspect, zNear, zFar;
  fovy = glm::radians(fov_deg);
  aspect = ((float)width) / ((float)height);
  zNear = n;
  zFar = f;
  glm::mat4 M_per = glm::perspective(fovy, aspect, zNear, zFar);
  return M_per;
}

/*
 * Inputs:
 *  glm::vec3 g : gaze vector (i.e. direction the eye is looking)
 *  glm::vec3 t : the "up" vector - points to the up direction of the world
 *  glm::vec3 e : location of the eye in world coords
 *
 * Outputs:
 *  glm::mat4 &M_cam : the Camera Transformation Matrix
 */
glm::mat4 mat::view(const glm::vec3 &g,
               const glm::vec3 &t,
               const glm::vec3 &e)
{
  // Compute the camera basis
  glm::vec3 w, u, v;
  w = -glm::normalize(g);
  u = glm::normalize(glm::cross(t, w));
  v = glm::cross(w, u);

  // M_cam = [ u v w e ]^-1, where u, v, w are
  // vec4 with the 4th index set to 0.
  glm::mat4 M_cam = glm::inverse(glm::mat4(
    glm::vec4(u,0),
    glm::vec4(v,0),
    glm::vec4(w,0),
    glm::vec4(e,1)
  ));
  
#if _DEBUG_MAT_LOGS_
  print_vec3("W:",w);
  print_vec3("U:",u);
  print_vec3("V:",v);
  print_mat4("M_cam:",M_cam);
#endif
  return M_cam;
}

glm::mat3 mat::rot(const float &theta,
                   const glm::vec3 &a)
{
  // determine rotational basis
  glm::vec3 w, u, v;
  w = glm::normalize(a);

  /*
   * (1) Construct an arbitrary vector t which is not colinear
   *     with w.
   * (2) Find u by taking the cross product of t and w. This
   *     works since t and w are not colinear (i.e. not on the
   *     same line), their cross product would form a vector
   *     which is perpendicular to w.
   */
  glm::vec3 t = w;
  int minInd = 0;
  {
    GLfloat min = (GLfloat)std::numeric_limits<float>::max();
    int i;
    for (i=0; i<3; i++) {
      if (w[i] < min) {
        minInd = i;
        min = w[i];
      }
    } 
  } 
  t[minInd] = 1;

  u = glm::normalize(glm::cross(t,w));
  v = glm::cross(w, u);

  // construct rotation matrix
  glm::mat3 R_inv(u, v, w);
  glm::mat3 R = glm::transpose(R_inv);
  glm::mat3 Rot = glm::transpose(glm::mat3(
    cos(theta), -sin(theta), 0,
    sin(theta), cos(theta), 0,
    0, 0, 1
  ));

  return R_inv*Rot*R;
}
