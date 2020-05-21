#ifndef __RENDER_HELPERS_H__
#define __RENDER_HELPERS_H__

#include <glad/glad.h>
#include <math.h>
#include <limits>
#include <glm/mat4x4.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/geometric.hpp>
#include <stdio.h>
#include <vector>
#include <sstream>
#include <iostream>

#include "lib.hpp"

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


inline void print_vbo(const std::vector<ld_o::VBO_STRUCT> &data) {
  int i;
  for (i=0; i<data.size(); i++) {
    const ld_o::VBO_STRUCT *v = &data[i];
    printf("[%d]\n", i);
    print_vec3("\tv:", v->v);
    print_vec3("\tn:", v->n);
    print_vec2("\tt:", v->t);
  }
}

inline glm::vec3 parse_vec3(std::string str) {
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

#endif /* __RENDER_HELPERS_H__ */
