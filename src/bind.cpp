#include <glad/glad.h>
#include "lib.h"

void bind_tex_fbo(const GLuint &TEX, GLuint &FBO) {
  GLuint _FBO;
  
  glGenFramebuffers(1, &_FBO);
  glBindFramebuffer(GL_FRAMEBUFFER, _FBO);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
    GL_TEXTURE_2D, TEX, 0);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  FBO = _FBO;
}

void bind_tex(const unsigned char *data,
						  const int width,
						  const int height,
						  const GLuint textureUnit,
						  GLuint &TEX) {
	GLuint _TEX;
	glGenTextures(1, &_TEX);
	glActiveTexture(textureUnit);
	glBindTexture(GL_TEXTURE_2D, _TEX);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 
		height, width, 0, 
		GL_RGB, GL_UNSIGNED_BYTE, data);

	TEX = _TEX;
	printf("Initialized texture\n");
}

void bind_vao(const std::vector<ld_o::VBO_STRUCT> &data, GLuint &VAO) {
  /*
   not exactly sure why we have to call glBindBuffer twice.
   glGenBuffers is like malloc
   the allocated buffer space is passed back in "triangleVBO",
   - e.g. we pass in an empty array
   NOTE: this is not actually an allocation of space, it is just
   creating a "handle" (as OpenGL calls it), the allocation of 
   space is specified later
   - OpenGL allocates "n" (in this case 1) buffer spaces
   - OpenGL passes the names of these allocated buffers back 
     by setting them in "triangleVBO"
   - e.g. triangleVBO[0] will have the name of the buffer we 
     asked for (i.e. the 1st allocated buffer will be at index
     0, 2nd at index 1, ...)
  */
  GLuint VBO;
  glGenBuffers(1, &VBO);

  /*
   https://stackoverflow.com/questions/8691663/can-you-use-multiple-targets-with-a-single-vbo
   Think of "targets" as slots in OpenGL.
  
   There are several buffer slots available in OpenGL.
   One of them is GL_ARRAY_BUFFER.
   glBindBuffer says to "bind" GL_ARRAY_BUFFER to
   the buffer handle created by the previous glGenBuffer
   call (i.e. triangleVBO[0])
   -------------------------------
   Imagine OpenGL has a global variable for the currently
   bound buffer.
   glBindBuffer essentially (or literally) changes this global
   variable.
   Any calls (such as glBufferData, glVertexAttribPointer, etc.)
   will all be applied to this global bound buffer.
   
   OpenGL's design is such that it does NOT allow its users to
   manipulate its underlying data structures. Intead, it provides
   fine-grained interfaces so that we are able to interact with
   its interals. --> this is a very common design pattern 
   (e.g. web applications have the same architecture)
  */
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  
  /* Send this data to GPU - it is bound to GL_ARRAY_BUFFER */
  size_t num_bytes = data.size()*sizeof(ld_o::VBO_STRUCT);
  glBufferData(GL_ARRAY_BUFFER, num_bytes, data.data(), GL_STATIC_DRAW);

  /*
   We need VAO so shaders can know how to format/parse our
   data on the GPU-side.
   VAO will make the connection between the data and shader
   input variable indexes.
  */
  GLuint _VAO;
  glGenVertexArrays(1, &_VAO);
  glBindVertexArray(_VAO);

  /* location = 0 */
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(
    0,                    /* Position 0 */ 
    3,                    /* Number of components per vertex attribute (i.e. vec3 = 3) */
    GL_FLOAT,             /* Type of each vertex attribute - GLfloat */
    GL_FALSE,             /* Is normalized? - no */
    sizeof(ld_o::VBO_STRUCT), /* Stride: byte offset between consecutive generic vertex attribs */
    0                     /* Offset (in bytes) of the first generic vertex attrib in the array */
  );

  /* location = 1 */
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(
    1,                    /* Position (layout) 1 */
    3,                    /* Number of components per vertex attrib */
    GL_FLOAT,             /* Type of each vertex attrib - GLfloat */
    GL_FALSE,             /* Is normalized? - no */
    sizeof(ld_o::VBO_STRUCT), /* Stride: byte offset b/w consecutive vertex attribs */
    (void *)(3*sizeof(float)) /* Offset (in bytes) of the first generic vertext attrib */
  );

  /* location = 2 */
  glEnableVertexAttribArray(2);
  glVertexAttribPointer(
    2,                    /* Position (layout) 1 */
    2,                    /* Number of components per vertex attrib */
    GL_FLOAT,             /* Type of each vertex attrib - GLfloat */
    GL_FALSE,             /* Is normalized? - no */
    sizeof(ld_o::VBO_STRUCT), /* Stride: byte offset b/w consecutive vertex attribs */
    (void *)(6*sizeof(float)) /* Offset (in bytes) of the first generic vertext attrib */
  );

  /*
   Unbind the buffer (0 is a reserved buffer handle - basically means
   NULL)
   Unbind the vertex array (same as buffer)
  */
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);

  printf("Initialized VAO\n");
  VAO = _VAO;
}

unsigned char *load_tex(const std::string &filepath,
                        int &width,
                        int &height) {
  unsigned char *data;
  /*
   * Basically we just have to reverse the bytes
   * to make it bottom up - since OpenGL processes
   * textures from the bottom-left pixel to the
   * top-right pixel (rather than top-left to bottom-
   * right), like usual
   */
  cimg_library::CImg<unsigned char> img;
  img.load(filepath.c_str());

  int N = clamp_dim(img.height());
  int M = clamp_dim(img.width());
  printf("Image loaded: %dx%d\n", N, M);

  data = (unsigned char *)malloc(3*N*M*sizeof(unsigned char));;
  int x, y, _y;
  for (y=0, _y=N-1; y<N; y++, _y--) {
    for (x=0; x<M; x++) {
      unsigned char r, g, b;
      int loc = LOC(x,y,M);

      r = img(x, _y, 0, 0);
      g = img(x, _y, 0, 1);
      b = img(x, _y, 0, 2);

      data[loc] = r;
      data[loc+1] = g;
      data[loc+2] = b;
    }
  }

  printf("Image transferred\n");
  width = M;
  height = N;
  return data;
}

void bind_shaders(const std::vector<ShaderProg> &shader_progs,
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
