#ifndef GCC_TEST_TRANSFORMATION_MATRICES
#define GCC_TEST_TRANSFORMATION_MATRICES

#include <glm/glm.hpp>
#include <glm/mat4x4.hpp>
#include <glm/geometric.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <stdio.h>
#include <iostream>
#include <cmath>

#include "helpers.h"

#define _DEBUG_MAT_LOGS_ 0 

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
void init_per_mat(const GLfloat &bot,
                    const GLfloat &top,
                    const GLfloat &left,
                    const GLfloat &right,
                    const GLfloat &near,
                    const GLfloat &far,
                    glm::mat4 &M_per) {
  #define r right
  #define l left
  #define t top
  #define b bot
  #define n near
  #define f far
  glm::mat4 M_orth, P;
  M_orth = glm::transpose(glm::mat4(
    2/(r-l),       0,       0, -(r+l)/(r-l),
          0, 2/(t-b),       0, -(t+b)/(t-b),
          0,       0, 2/(n-f), -(n+f)/(n-f),
          0,       0,       0,            1
  ));
  P = glm::transpose(glm::mat4(
    n, 0, 0, 0,
    0, n, 0, 0,
    0, 0, n+f, -n*f,
    0, 0, 1, 0
  ));

  M_per = M_orth * P;

  #if _DEBUG_MAT_LOGS_
    print_mat4("M_orth:", M_orth);
    print_mat4("P:", P);
    print_mat4("M_per:", M_per);
    print_vec4("M_per*[0 0 -2 1]:", M_per*glm::vec4(0,0,-2,1));
    print_vec4("P*[0 0 -2 1]:", P*glm::vec4(0,0,-2,1));

    // Check perspective matrix is correct
    glm::vec4 _t(1,2,3,1);
    glm::vec4 _r = P * _t;
    assert(_r[0] == n*_t[0]);
    assert(_r[1] == n*_t[1]);
    assert(_r[2] == (n+f)*_t[2]-f*n);
    assert(_r[3] == _t[2]);
    
    assert(M_per == glm::transpose(glm::mat4(
      (2*n)/(r-l), 0, (l+r)/(l-r), 0,
      0, (2*n)/(t-b), (b+t)/(b-t), 0,
      0, 0, (f+n)/(n-f), (2*f*n)/(f-n),
      0, 0, 1, 0
    )));
  #endif
  
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
  M_per = glm::perspective(
    glm::radians(30.0f),
    1400.0f/900.0f,
    0.1f, 100.0f 
  );
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
void init_camera_mat(const glm::vec3 &g,
                     const glm::vec3 &t,
                     const glm::vec3 &e,
                     glm::mat4 &M_cam) {
  // Compute the camera basis
  glm::vec3 w, u, v;
  w = -glm::normalize(g);
  u = glm::normalize(glm::cross(t, w));
  v = glm::cross(w, u);

  // M_cam = [ u v w e ]^-1, where u, v, w are
  // vec4 with the 4th index set to 0.
  M_cam = glm::inverse(glm::mat4(
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
}

#endif /* GCC_TEST_TRANSFORMATION_MATRICES */
