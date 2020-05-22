#include <string>
#include <sstream>
#include <fstream>
#include <nlohmann/json.hpp>
#include <glm/geometric.hpp>
#include <glm/mat4x4.hpp>
#include <glm/glm.hpp>

#include "lib.hpp"
#include "types.hpp"
#include "helpers.h"

#define SCENE_DEBUG 1
using json = nlohmann::json;


Scene::Scene(std::string n, int w, int h)
  : filename(n)
  , WIDTH(w)
  , HEIGHT(h)
{
  std::ifstream ifs(filename);
  if (!ifs.is_open()) {
    printf("Failed to open file: %s\n", filename.c_str());
    exit(1);
  }

  std::stringstream buf;
  buf << ifs.rdbuf();

  json j = json::parse(buf.str()); 

  // Miscellaneous
  {
    this->orient = new Orientation{
      j["eye"].get<std::string>(), 
      j["gaze"].get<std::string>(), 
      j["top"].get<std::string>(),
      j["fovy"].get<float>(),
      j["zNear"].get<float>(),
      j["zFar"].get<float>()
    };

    this->Ia_ = parse_vec3(j["Ia"].get<std::string>()); 
    this->Kd_ = parse_vec3(j["Kd"].get<std::string>()); 
    this->Ka_ = parse_vec3(j["Ka"].get<std::string>()); 
    this->Ks_ = parse_vec3(j["Ks"].get<std::string>()); 
    this->p = j["p"].get<int>();
  }

  // Lights
  {
    auto lights = j["lights"];
    assert(lights.is_array());

    json lightJson;
    int i;
    for (i=0; i<lights.size(); i++) {
      lightJson = lights[i];
      Light light(
        lightJson["position"].get<std::string>(),
        lightJson["intensity"].get<std::string>(),
        this->orient->top,
        this->WIDTH,
        this->HEIGHT       
      );
      this->lights_.push_back(light); 
    }
  }

  // Objects
  {
    auto objects = j["objects"];
    assert(objects.is_array());

    std::string id, filename;
    Data *data; 
    json objectJson;
    int i;
    for (i=0; i<objects.size(); i++) {
      objectJson = objects[i];
      id = objectJson["id"].get<std::string>();
      filename = objectJson["filename"].get<std::string>();

      data = new Data(filename.c_str());
      this->objects[id] = data; 
    }
  }

  // Textures
  {
    auto textures = j["textures"];
    assert(textures.is_array());

    std::string id, filename;
    Texture *tex;
    json texJson;
    int i;
    for (i=0; i<textures.size(); i++) {
      texJson = textures[i];
      id = texJson["id"].get<std::string>();
      filename = texJson["filename"].get<std::string>();

      tex = new Texture(filename);
      this->textures[id] = tex;
    }
  }

  // Models
  {
    auto models = j["models"];
    assert(models.is_array());

    std::string object_id, tex_id;
    Model *model;
    Texture *tex;
    json modelJson;
    int i;
    for (i=0; i<models.size(); i++) {
      modelJson = models[i];
      object_id = modelJson["object_id"].get<std::string>();
      assert(this->objects.count(object_id) == 1);

      tex = NULL;
      if (modelJson.contains("tex_id")) {
        tex_id = modelJson["tex_id"].get<std::string>();
        assert(this->textures.count(tex_id) == 1);
        tex = this->textures[tex_id];
      }

      model = new Model(
        this->objects[object_id],
        tex, 
        modelJson["rotation_deg"].get<float>(),
        modelJson["rotation_axis"].get<std::string>(),
        modelJson["scale"].get<std::string>(),
        modelJson["translate"].get<std::string>()
      );
      this->models.push_back(model);
    }
  }

  // Shadow Map
  {
    auto programs = j["programs"];

    std::string nvs, nfs;
    nvs = programs["shadow-vs"].get<std::string>();
    nfs = programs["shadow-fs"].get<std::string>();

    this->shadowMap = new ShadowMap(
      this->lights_,
      this->models,
      this->WIDTH, this->HEIGHT,
      nvs, nfs
    );
  }
}

const GLfloat *Scene::Kd() {return (const GLfloat *)&Kd_; }
const GLfloat *Scene::Ka() {return (const GLfloat *)&Ka_; }
const GLfloat *Scene::Ks() {return (const GLfloat *)&Ks_; }
const GLfloat *Scene::Ia() {return (const GLfloat *)&Ia_; }

void Scene::ld_lights_uniform(const GLuint prog,
                              const char *posFmtStr,
                              const char *intensityFmtStr,
                              const char *shadowMatFmtStr,
                              const GLuint startIndex) 
{
  char id_name[100];
  Light *light;
  int texUnitId;
  int i;
  for (i=0; i<lights_.size(); i++) {
    int id;
    light = &lights_[i];
    snprintf(id_name, 100, posFmtStr, i);
    id = glGetUniformLocation(prog, id_name);   
    glUniform3fv(id, 1, (const GLfloat *)&light->position);

    snprintf(id_name, 100, intensityFmtStr, i);
    id = glGetUniformLocation(prog, id_name);   
    glUniform3fv(id, 1, (const GLfloat *)&light->intensity);

    snprintf(id_name, 100, shadowMatFmtStr, i);
    id = glGetUniformLocation(prog, id_name);
    glm::mat4 shadowMat = light->Mvp_bias();
    glUniformMatrix4fv(id, 1, false, glm::value_ptr(shadowMat));
  } 
}
