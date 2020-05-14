#ifndef __RENDER_TEXTURE_H__
#define __RENDER_TEXTURE_H__

#include <glad/glad.h>
#include <string>
#include "transformation_matrices.h"
#include "_v1/load_tex.h"

class Texture {
  public:
    unsigned char *data = NULL;
    std::string file;
    int width;
    int height;
    glm::mat4 per, view;
    GLuint fbo_id;
    GLuint id;

    Texture(std::string f) : file(f) {
      glGenTextures(1, &id);
      data = load_tex(file, width, height);
    }
    Texture(unsigned char *d, int w, int h) 
      : data(d), width(w), height(h) 
    {
      glGenTextures(1, &id);
      glGenFramebuffers(1, &fbo_id);
    }
    Texture() {}
    ~Texture() {
      if (data != NULL) {
        free(data);
      }
      glDeleteTextures(1, &id);
    }
    void framebuffer() {
      glBindFramebuffer(GL_FRAMEBUFFER, fbo_id);

      glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
        GL_TEXTURE_2D, id, 0);

      glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
    void bind2D_RGB() {
      glBindTexture(GL_TEXTURE_2D, id);

      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 
        height, width, 0, 
        GL_RGB, GL_UNSIGNED_BYTE, data);

      glActiveTexture(0);
      glBindTexture(GL_TEXTURE_2D, 0);
    }
    void bind_to_unit(GLuint i) {
      glActiveTexture(GL_TEXTURE0 + i);
      {
        GLint _i;
        glGetIntegerv(GL_ACTIVE_TEXTURE, &_i);
        assert(_i == (GL_TEXTURE0 + i));        
      }
      glBindTexture(GL_TEXTURE_2D, id);
      {
        GLint _i;
        glGetIntegerv(GL_TEXTURE_BINDING_2D, &_i);
        assert(_i == id);        
      }
    }
    void shadow_map(const GLuint prog_id,
                    const Data &data,
                    const glm::vec3 &g, 
                    const glm::vec3 &t,
                    const glm::vec3 &e) {
      {
        init_camera_mat(g, t, e, view);
        per = glm::perspective(
          glm::radians(30.0f),
          1400.0f/900.0f,
          0.1f, 100.0f 
        );
      }

      glEnable(GL_DEPTH_TEST);
      glClearColor(1.0f,1.0f,1.0f,1.0f);
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

      // Bind the shaders
      glUseProgram(prog_id);
      glBindFramebuffer(GL_FRAMEBUFFER, fbo_id);
      glBindVertexArray(data.vao);

      GLuint per_id, view_id;
      GLuint e_id;
      per_id = glGetUniformLocation(prog_id, "per");
      view_id = glGetUniformLocation(prog_id, "view");
      e_id = glGetUniformLocation(prog_id, "light");
      glUniformMatrix4fv(per_id, 1, false, glm::value_ptr(per));
      glUniformMatrix4fv(view_id, 1, false, glm::value_ptr(view));
      glUniform3fv(e_id, 1, glm::value_ptr(e));

      glDrawArrays(GL_TRIANGLES, 0, data.data.size());

      // Resetting GL to defaults
      glBindVertexArray(0);
      glBindFramebuffer(GL_FRAMEBUFFER, 0);
      glUseProgram(0);
      glDisable(GL_DEPTH_TEST);
    }
};

#endif /* __RENDER_TEXTURE_H__ */