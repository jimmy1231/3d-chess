#include <math.h>
#include <vector>
#include <string.h>
#include <fstream>
#include <sstream>
#include <iostream>
#include <stdio.h>
#include <chrono>
#include <thread>

/*
 glad is the OpenGL library loader, basically, it discovers where all the
 openGL functions are in the process memory, then dynamically binds it
 to the invocation addresses (either through PLT or GOT).
 They do something clever like this:
 
 #define glCompileProgram glad_glCompileProgram
 So, when we call glCompileProgram(), it is actually invoking
 the glad_glCompileProgram function 
*/
#include <CImg/CImg.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/mat4x4.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/geometric.hpp>

#include "helpers.h"
#include "lib.hpp"
#include "mat.hpp"

// Custom header files
#include "ShaderProg.h"
#include "types.hpp"

#define _DEBUG_LOOP_LOGS_ 0
// #define DEBUG_MODE

#ifndef DEBUG_MODE
  #define FPS 60
#else
  #define FPS 1 
#endif

#define MAX_MS_PER_FRAME 1000 / FPS /* milliseconds per frame */
#define WIDTH_PIXELS 1400 
#define HEIGHT_PIXELS 900 
#define DEG_TO_RAD 0.0174533

#define time_p std::chrono::high_resolution_clock::time_point
#define duration std::chrono::duration
#define clock std::chrono::high_resolution_clock
#define duration_cast std::chrono::duration_cast

Scene scene;
glm::vec2 *last_cursor_pos = NULL;
bool LEFT_MOUSE_BTN_PRESSED = false;

void window_callback_scroll(const double &xoffset, const double &yoffset) {
  // Get direction of gaze, and adjust towards/away that direction
  Orientation *orient = scene.orient;
  glm::vec3 _d = 0.2f * glm::normalize(orient->gaze);
  
  if (yoffset > 0) {
    orient->eye = orient->eye+_d; 
  } else {
    orient->eye = orient->eye-_d;
  }
}

void window_callback_mouse_btn(int btn, int action, int mods) {
  if (btn == GLFW_MOUSE_BUTTON_LEFT) {
    if (action == GLFW_PRESS) {
      LEFT_MOUSE_BTN_PRESSED = true;
    } else {
      assert(action == GLFW_RELEASE);
      LEFT_MOUSE_BTN_PRESSED = false;
      free(last_cursor_pos);
      last_cursor_pos = NULL;
    }
  }
  /* Add more mouse events here (i.e. GLFW_MOUSE_BUTTON_RIGHT) */
}

void window_callback_cursor_pos(float xpos, float ypos) {
  if (!LEFT_MOUSE_BTN_PRESSED) {
    return;
  }

  glm::vec2 curr_cursor_pos = glm::vec2(xpos, ypos);
  if (last_cursor_pos == NULL) {
    last_cursor_pos = (glm::vec2 *)malloc(sizeof(glm::vec2));
    *last_cursor_pos = glm::vec2(xpos, ypos);
  }
  
  #define curr curr_cursor_pos
  #define prev last_cursor_pos
  glm::vec2 delta = curr-(*prev);

  /*
   * left (delta.x < 0) -> clockwise
   * right (delta.x > 0) -> counter-clockwise
   * Treat delta.x (in pixels) as degrees - see how it works out
   *  - add a little smoothing by capping the angle
   */
  
  // Rotate about y-axis by rad
  Orientation *orient = scene.orient;

  float rad;
  glm::vec3 axis;
  glm::mat3 R;
  if (fabs(delta.x) > fabs(delta.y)) { 
    rad = fmin((float)(DEG_TO_RAD * -delta.x)/10.0f, 0.5f);
    axis = glm::vec3(0,1,0);
  } else {
    rad = fmin((float)(DEG_TO_RAD * -delta.y)/10.0f, 0.5f);

    // Rotate about 
    glm::vec3 u(glm::cross(orient->gaze, orient->top));
    axis = normalize(u);
  }
  
  R = mat::rot(rad, axis);
  orient->eye = R*orient->eye;
  orient->gaze = glm::normalize(
    glm::vec3(0.0f,0.0f,0.0f)-orient->eye);

  // Finally, reset prev (i.e. last_cursor_pos)
  *prev = curr;
}

int main(int argc, char *argv[]) {
  if (argc != 2) {
    std::cout << "Please specify input file" << std::endl;
    exit(0);
  }

  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
  glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

  GLFWwindow *window = glfwCreateWindow(WIDTH_PIXELS, HEIGHT_PIXELS,
    "OpenGL", NULL, NULL) ;
  glfwMakeContextCurrent(window);

  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    printf("FAILED TO LOAD GLAD!\n"); 
    return EXIT_FAILURE;
  }

  //////////////////////////////////////////////////////////////////////
  scene = {argv[1], WIDTH_PIXELS, HEIGHT_PIXELS};
  //////////////////////////////////////////////////////////////////////

  auto ld_shaders = [](const char *nvs, const char *nfs)
    -> GLuint {
    // Load shadow map shaders
    GLuint prog_id;
    std::vector<ShaderProg> progs;
    ShaderProg vs, fs;
    vs = {nvs, GL_VERTEX_SHADER};
    fs = {nfs, GL_FRAGMENT_SHADER};
    progs.push_back(vs);
    progs.push_back(fs);
    bind_shaders(progs, prog_id);
    return prog_id;
  };

  /*
   * Note: we do not need the M_vp (i.e. viewport transformation)
   * because OpenGL does this automatically for us in the
   * vertex processing stage (right after the vertex shader).
   * It applies the viewport transformation since it knows 
   * the screen width, height, and depth of our vertices.
   *
   * Note: the perspective (i.e. projection) matrix does not
   * change in between frames, so we can create it once, and
   * reuse it.
  */
  scene.ld_shadow_maps("../glsl/shadow-map.vs", 
                      "../glsl/shadow-map.fs");
  GLuint prog_id = ld_shaders("../glsl/model-view-proj.vs", 
                              "../glsl/per-frag-blinn-phong.fs");

  glfwSetScrollCallback(
    window,
    [](GLFWwindow *window, double xoffset, double yoffset) {
      window_callback_scroll(xoffset, yoffset);
    }
  );

  glfwSetMouseButtonCallback(
    window,
    [](GLFWwindow *window, int btn, int action, int mods) {
      window_callback_mouse_btn(btn, action, mods);
    }
  );

  glfwSetCursorPosCallback(window,
    [](GLFWwindow *window, double xpos, double ypos) {
      window_callback_cursor_pos((float)xpos, (float)ypos);
    }
  );
  
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE);
  glCullFace(GL_BACK);
  time_p tic, toc;
  duration<int, std::milli> fps(MAX_MS_PER_FRAME);
  while (!glfwWindowShouldClose(window)) {
    tic = clock::now();
    glClearColor(46.0f/255.0f, 56.0f/255.0f, 71.0f/255.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Bind the shaders
    glUseProgram(prog_id);

    GLint M_proj_id, M_per_id, M_cam_id;
    GLint ks_id, kd_id, ka_id;
    GLint Ia_id;
    GLint num_lights_id;
    GLint p_id;
    GLint M_scale_bias_id;
    M_per_id = glGetUniformLocation(prog_id, "M_per");
    M_cam_id = glGetUniformLocation(prog_id, "M_cam");
    M_scale_bias_id = glGetUniformLocation(prog_id, "M_scale_bias");
    ks_id = glGetUniformLocation(prog_id, "ks");
    kd_id = glGetUniformLocation(prog_id, "kd");
    ka_id = glGetUniformLocation(prog_id, "ka");
    num_lights_id = glGetUniformLocation(prog_id, "num_lights");
    Ia_id = glGetUniformLocation(prog_id, "Ia");
    p_id = glGetUniformLocation(prog_id, "p");

    // Send uniform variables to device
    glUniformMatrix4fv(M_per_id, 1, false, 
      scene.orient->perspective(WIDTH_PIXELS, HEIGHT_PIXELS));
    glUniformMatrix4fv(M_cam_id, 1, false, scene.orient->view());
    glUniformMatrix4fv(M_scale_bias_id, 1, false, scene.orient->scale_bias());
    glUniform3fv(ks_id, 1, scene.Ks());
    glUniform3fv(kd_id, 1, scene.Kd());
    glUniform3fv(ka_id, 1, scene.Ka());
    glUniform3fv(Ia_id, 1, scene.Ia());
    glUniform1i(num_lights_id, scene.lights_.size());
    glUniform1f(p_id, scene.p);
    scene.ld_lights_uniform(prog_id, 
                           "lights[%d].position",
                           "lights[%d].intensity",
                           "lights[%d].shadow",
                           "lights[%d].shadowMat",
                           1);

    Texture *tex;
    Data *data;
    GLint M_model_id, tex_id;
    for (Model *&model : scene.models) {
      data = model->data_;
      tex = model->tex_;
      data->bind_VAO();
      glBindVertexArray(data->vao);

      M_model_id = glGetUniformLocation(prog_id, "M_model");

      // Bind texture for model
      if (tex != NULL) {
        tex_id = glGetUniformLocation(prog_id, "tex");
        glUniformMatrix4fv(M_model_id, 1, false, model->model());
        glUniform1i(tex_id, 0);
        tex->bind_to_unit(0);
      }
      glDrawArrays(GL_TRIANGLES, 0, data->size());
    } 

    // Unbind the shaders
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindVertexArray(0);
    glUseProgram(0);

    glfwSwapBuffers(window);
    glfwPollEvents();

    toc = clock::now();
    auto elapsed_ms = duration_cast<std::chrono::milliseconds>(toc-tic);
    if (elapsed_ms < fps) {
      auto left = fps-elapsed_ms;
      std::this_thread::sleep_for(left);
    } 
  }

  glfwTerminate();
}
