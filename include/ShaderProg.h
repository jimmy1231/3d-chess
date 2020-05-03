#ifndef GCC_TEST_SHADER_PROG
#define GCC_TEST_SHADER_PROG

typedef struct {
  std::string shader_name;
  GLuint type;
  GLchar *glsl_code;
} ShaderProg;

#endif
