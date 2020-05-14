#ifndef __RENDER_TEXTURE_H__
#define __RENDER_TEXTURE_H__

#include <string>

class Texture {
  unsigned char *data = NULL;
  std::string file;
  int width;
  int height;
  GLuint fbo_id;
  GLuint id;

  public:
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
    Texture()
    ~Texture() {
      if (data != NULL) {
        free(data);
      }
    }
    int height() { return height; }
    int width() { return width; }
    GLuint id() { return id; }
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

      glBindTexture(GL_TEXTURE_2D, 0);
    }
    void shadow_map(const GLuint prog_id,
                    const Data &data,
                    const glm::vec3 &g, 
                    const glm::vec3 &t,
                    const glm::vec3 &e) {
      glm::mat4 per, view;
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
      glBindVertexArray(data.vao());

      GLuint per_id, view_id;
      GLuint light_id;
      per_id = glGetUniformLocation(prog_id, "per");
      view_id = glGetUniformLocation(prog_id, "view");
      light_id = glGetUniformLocation(light_id, "light");
      glUniformMatrix4fv(per_id, 1, false, glm::value_ptr(per));
      glUniformMatrix4fv(view_id, 1, false, glm::value_ptr(view));
      glUniform3fv(light_id, 1, glm::value_ptr(light));

      glDrawArrays(GL_TRIANGLES, 0, data.size());

      // Resetting GL to defaults
      glBindVertexArray(0);
      glBindFramebuffer(GL_FRAMEBUFFER, 0);
      glUseProgram(0);
      glDisable(GL_DEPTH_TEST);
    }
}

#endif /* __RENDER_TEXTURE_H__ */