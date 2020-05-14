#ifndef GCC_TEST_BIND_SHADERS
#define GCC_TEST_BIND_SHADERS

#include <stdio.h>
#include <iostream>
#include <sstream>
#include <glad/glad.h>
#include "ShaderProg.h"

void 
bind_shaders(const std::vector<ShaderProg> &shader_progs,
                  GLuint &prog_id)
{
  std::vector<GLuint> shaders;

  GLuint s;
  GLint status;
  int i;
  for (i=0; i<shader_progs.size(); i++) {
    const ShaderProg *shader_prog = &shader_progs[i]; 

    s = glCreateShader(shader_prog->type);
    GLchar *code = (GLchar *)shader_prog->code.c_str();
    glShaderSource(s, 1, &code, 0);
    glCompileShader(s);
     
    // Check shader compile status
    glGetShaderiv(s, GL_COMPILE_STATUS, &status);
    if (!status) {
      GLchar *error_log;
      GLint log_length;
      glGetShaderiv(s, GL_INFO_LOG_LENGTH, &log_length);

      error_log = (GLchar *)malloc(log_length*sizeof(GLchar));
      glGetShaderInfoLog(s, log_length, &log_length, error_log);
      printf("Shader <%s> failed to compile: %s\n",
        shader_prog->filename.c_str(),
        error_log);
      
      // clean up error log (free)
      int _i;
      for (_i=0; _i<shaders.size(); i++) {
        glDeleteShader(shaders[_i]);
      }
      free(error_log);

      return;
    } 
    
    printf("shader compiled successfully: %s\n",
      shader_prog->filename.c_str());
    shaders.push_back(s);
  } 

  // finally, link all shaders to program
  GLuint prog = glCreateProgram();
  for (i=0; i<shaders.size(); i++) {
    glAttachShader(prog, shaders[i]);
  }
  glLinkProgram(prog);

  // Check for errors
  glGetProgramiv(prog, GL_LINK_STATUS, (int *)&status);
  if (!status) {
    GLchar *error_log;
    GLint log_length;
    glGetProgramiv(prog, GL_INFO_LOG_LENGTH, &log_length);

    error_log = (GLchar *)malloc(log_length*sizeof(GLchar));
    glGetProgramInfoLog(prog, log_length, &log_length, error_log);
    printf("Error linking program: %s\n", error_log);

    // clean up resources
    glDeleteProgram(prog);
    int _i;
    for (_i=0; i<shaders.size(); _i++) {
      glDeleteShader(shaders[_i]);
    }
    free(error_log);
    
    return;
  }

  prog_id = prog;
}

#endif
