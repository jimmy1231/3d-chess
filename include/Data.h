#ifndef __RENDER_DATA_H__
#define __RENDER_DATA_H__

#include "load_obj.h"

class Data {
  public:
    std::string filename; 
    std::vector<ld_o::VBO_STRUCT> data;
    GLuint vbo;
    GLuint vao;

    Data(char *f) : filename(f) {
      /*
       we can define this locally in this function because GL
       will copy all of this to GPU anyways, we do not need
       this data beyond defining it as a means to be copied.
       Note: this vertices data is already in screen coordinates,
       therefore, it is meant only for the fragment shader
       (i.e. no need for vertex processing)
      */
      load_obj(filename, data);
      glGenBuffers(1, &vbo);
      
      glBindBuffer(GL_ARRAY_BUFFER, vbo);

      size_t nbytes;
      nbytes = data.size()*sizeof(ld_o::VBO_STRUCT);
      glBufferData(GL_ARRAY_BUFFER, nbytes, data.data(), GL_STATIC_DRAW);
      
      glBindBuffer(GL_ARRAY_BUFFER, 0);      
    }
    void bind_VAO() {
      glGenVertexArrays(1, &vao); 
      glBindBuffer(GL_ARRAY_BUFFER, vbo);
      glBindVertexArray(vao);
      
      size_t stride = sizeof(ld_o::VBO_STRUCT);
      glEnableVertexAttribArray(0);
      glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, 0);
      
      glEnableVertexAttribArray(1);
      glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, 
        (void *)(3*sizeof(float)));

      glEnableVertexAttribArray(2);
      glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride, 
        (void *)(6*sizeof(float)));

      glBindBuffer(GL_ARRAY_BUFFER, 0);
      glBindVertexArray(0);
    }
};

#endif /* __RENDER_DATA_H__ */
