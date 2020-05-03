#ifndef GCC_TEST_LOAD_SHADER_PROG
#define GCC_TEST_LOAD_SHADER_PROG

#include <vector>
#include <string.h>
#include <fstream>
#include <sstream>
#include <iostream>
#include <stdio.h>
#include "ShaderProg.h"
#include <glad/glad.h>

void load_shader_prog(ShaderProg &prog) {
  std::ifstream ifs(prog.shader_name);
  if (!ifs.is_open()) {
    printf("Failed to open file: %s\n", prog.shader_name.c_str());
    return;
  }
  std::stringstream buf;
  buf << ifs.rdbuf();
  
  std::string str = buf.str();
  size_t len = str.size();
  const char *prog_str = str.c_str();

  prog.glsl_code = (GLchar *)malloc(len*sizeof(GLchar));
  strcpy((char *)prog.glsl_code, prog_str);  
}

#endif
