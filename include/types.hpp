#ifndef __RENDER_TYPES_H__
#define __RENDER_TYPES_H__

#include <string>
#include <sstream>
#include <fstream>
#include <nlohmann/json.hpp>
#include <glm/geometric.hpp>
#include <glm/mat4x4.hpp>
#include <glm/glm.hpp>

#include "lib.hpp"
#include "types_decl.h"


class Scene {
  public:
    int WIDTH, HEIGHT;

    std::string filename;

    std::vector<Light> lights_;
    Orientation *orient;
    glm::vec3 Ia_;
    glm::vec3 Kd_;
    glm::vec3 Ks_;
    glm::vec3 Ka_;
    int p;

    ShadowMap *shadowMap;

    std::unordered_map<std::string, Texture *> textures;
    std::unordered_map<std::string, Data *> objects;
    std::vector<Model *> models;

    Scene() {}
    Scene(std::string, int, int);

    const GLfloat *Kd();
    const GLfloat *Ka();
    const GLfloat *Ks();
    const GLfloat *Ia();

    void ld_lights_uniform(const GLuint,
                           const char *position,
                           const char *intensity,
                           const char *shadowMap,
                           const GLuint offset);
};

class ShadowMap {
	static GLenum DRAW_BUFFERS[1];
	public:
		GLuint fbo;
		GLuint tex;

		ShadowMap(std::vector<Light> &lights,
						  std::vector<Model *> &models,
							int width,
							int height,
							std::string nvs,
							std::string nfs);
};

class Model {
  public:
    glm::mat4 model_; 

    Texture *tex_;
    Data *data_;

    float rotationDeg_;
    glm::vec3 rotationAxis_;
    glm::vec3 scale_;
    glm::vec3 translate_;

    Model(Data *, Texture *, float, std::string, std::string, std::string);
    const GLfloat *model();
};

class Light {
  public:
    glm::mat4 per;
    glm::mat4 view;

    static const size_t Size = 6;
    glm::vec3 position;
    glm::vec3 intensity;

    Light(std::string p, std::string i, glm::vec3 t, int w, int h);
    glm::mat4 Mvp_bias();
    glm::mat4 Mvp();
};

class Texture {
  public:
    /* Which slot to attach the shadow texture to in FBO */
    GLenum mode;

    unsigned char *data = NULL;
    std::string file;
    int width;
    int height;
    GLuint fbo_id;
    GLuint id;

    int layers;

    Texture(std::string);
    Texture();
    ~Texture();
};

class Orientation {
  public:
    glm::mat4 view_;
    glm::mat4 per_;
    glm::mat4 scale_bias_;

    glm::vec3 eye;
    glm::vec3 gaze;
    glm::vec3 top;

    float fovy;
    float zNear;
    float zFar;

    Orientation();
    Orientation(std::string, std::string, std::string, float, float, float);
 
    const GLfloat *view();
    const GLfloat *scale_bias();
    const GLfloat *perspective(const float &, const float &);
};

class Data {
  public:
    std::string filename; 
    std::vector<ld_o::VBO_STRUCT> data;
    GLuint vbo;
    GLuint vao;

    Data(const char *);

    void print();
    size_t size();
};

#endif /* __RENDER_TYPES_H__ */