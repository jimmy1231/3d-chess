#ifndef GCC_TEST_SHADER_PROG
#define GCC_TEST_SHADER_PROG

class ShaderProg {
  std::string shader_name;
  GLuint type;
  std::string code;
	public:
    ShaderProg(std::string n, GLuint t) 
      : shader_name(n), type(t)
    {
      std::ifstream ifs(n);
      if (!ifs.is_open()) {
        printf("Failed to open file: %s\n", prog.shader_name.c_str());
        exit(1);
      }

      std::stringstream buf;
      buf << ifs.rdbuf();
      code = buf.str();
    }
    GLchar **code() { return &code.c_str(); }
    char *name() { return shader_name.c_str(); }
}

#endif
