#ifndef GCC_TEST_SHADER_PROG
#define GCC_TEST_SHADER_PROG

#include <string>
#include <fstream>
#include <cstring>
#include <sstream>

class ShaderProg {
	public:
    std::string filename;
    GLuint type;
    std::string code;

    ShaderProg() {}
    ShaderProg(std::string n, GLuint t) 
      : filename(n), type(t)
    {
      std::ifstream ifs(filename);
      if (!ifs.is_open()) {
        printf("Failed to open file: %s\n", filename.c_str());
        exit(1);
      }

      std::stringstream buf;
      buf << ifs.rdbuf();
      code = buf.str();
    }
};

#endif
