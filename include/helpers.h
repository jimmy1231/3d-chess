#ifndef GCC_TEST_HELPERS
#define GCC_TEST_HELPERS

#include <math.h>
#include <limits>
#include <glm/mat4x4.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/geometric.hpp>
#include <stdio.h>
#include <sstream>
#include <iostream>

#define PRINT_VEC(prefix,n,v) \
  {int pv_i; \
  std::cout << prefix << " ["; \
  for (pv_i=0; pv_i<n; pv_i++) { \
    printf("%5.3f", (float)v[pv_i]); \
    if (pv_i != n-1) { \
      std::cout << " "; \
    } \
  } \
  std::cout << "]" << std::endl;}

#define PRINT_MAT(prefix,n,m,mat) \
  std::cout << prefix << std::endl; \
  {int pm_i,pm_j; \
  for (pm_i=0;pm_i<m;pm_i++) { \
    std::cout << "["; \
    for (pm_j=0;pm_j<n;pm_j++) { \
      printf("%5.3f", (float)mat[pm_j][pm_i]); \
      if (pm_j != n-1) { \
        std::cout << " "; \
      } \
    } \
    std::cout << "]" << std::endl; \
  }} 

#define print_vec2(prefix,v) PRINT_VEC(prefix, 2, v)
#define print_vec3(prefix,v) PRINT_VEC(prefix, 3, v)
#define print_vec4(prefix,v) PRINT_VEC(prefix, 4, v)
#define print_mat3(prefix,m) PRINT_MAT(prefix, 3, 3, m)
#define print_mat4(prefix,m) PRINT_MAT(prefix, 4, 4, m)

glm::vec3 parse_vec3(std::string str) {
  std::vector<float> parts;
  std::string _s = str;
  int i=0, j=0;
  while (i < _s.length()) {
    j = _s.find(" ", i);
    if (j == std::string::npos) {
      j = _s.length();
    }

    std::string substr = _s.substr(i, j-i);
    float num = std::stof(substr);
    parts.push_back(num);

    i = j+1;
  }

  assert(parts.size() >= 3);
  glm::vec3 vec(parts[0], parts[1], parts[2]);
  return vec;
}

namespace gcc_test {
  glm::mat3 rot_about(const float &theta, const glm::vec3 &a) {
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
}

#endif

