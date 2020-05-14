#ifndef GCC_TEST_BIND_VAO
#define GCC_TEST_BIND_VAO

#include <glad/glad.h>
#include <glm/vec3.hpp>
#include <vector>

#include "load_obj.h"

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

#endif
