#include <string>
#include <sstream>
#include <fstream>
#include <nlohmann/json.hpp>
#include <glm/geometric.hpp>
#include <glm/mat4x4.hpp>
#include <glm/glm.hpp>

#include "lib.hpp"
#include "types.hpp"

#define SCENE_DEBUG 1
using json = nlohmann::json;


Scene::Scene(std::string n) : filename(n) {
  std::ifstream ifs(filename);
  if (!ifs.is_open()) {
    printf("Failed to open file: %s\n", filename.c_str());
    exit(1);
  }

  std::stringstream buf;
  buf << ifs.rdbuf();

  json j = json::parse(buf.str()); 

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
        lightJson["intensity"].get<std::string>()
      );
      this->lights_.push_back(light); 
    }
  }

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
}

void Scene::ld_shadow_maps(const std::string &nvs,
                           const std::string &nfs) {
  /*
   * Load shadow by pre-rendering shadow map into a 
   * texture framebuffer proxy. After this pre-render.
   * Since we bind the shadow map texture to the 
   * framebuffer, the resulting render (i.e. shadow
   * map) will be stored in a GPU buffer referenced by
   * our texture.
   * We can then use this texture to generate shadows
   * in our main render. Shadow map need not change
   * unless objects/lights in our scene moves, in which
   * case we need to re-render the shadow map.
   */

  // Load shadow map shaders
  GLuint prog_id;
  {
    std::vector<ShaderProg> progs;
    ShaderProg vs, fs;
    vs = {nvs, GL_VERTEX_SHADER};
    fs = {nfs, GL_FRAGMENT_SHADER};
    progs.push_back(vs);
    progs.push_back(fs);
    bind_shaders(progs, prog_id);
  }

  Light *light;
  int i;
  for (i=0; i<lights_.size(); i++) {
    light = &lights_[i];
    light->initShadow(prog_id, *this);
#if SCENE_DEBUG
    char screenshot_filename[30];
    snprintf(screenshot_filename, 30, "../shadow_map_%d.tga", i);

    screenshot(light->shadow->fbo_id,
               GL_DEPTH_ATTACHMENT,
               screen::IMAGE_TYPE_GREYSCALE,
               this->WIDTH, this->HEIGHT,
               screenshot_filename);
#endif
  } 
}

const GLfloat *Scene::Kd() {return (const GLfloat *)&Kd_; }
const GLfloat *Scene::Ka() {return (const GLfloat *)&Ka_; }
const GLfloat *Scene::Ks() {return (const GLfloat *)&Ks_; }
const GLfloat *Scene::Ia() {return (const GLfloat *)&Ia_; }

void Scene::ld_lights_uniform(const GLuint prog,
                              const char *posFmtStr,
                              const char *intensityFmtStr,
                              const char *shadowFmtStr,
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

    /*
     * Bind each light's shadow map to a preset location.
     * The reason we have 'startIndex' is to leave it up
     * to the caller to decide if they want to allocate 
     * texture units for other textures (e.g. normal maps,
     * model textures, etc.).
     */
    if (light->shadow != NULL) {
      snprintf(id_name, 100, shadowFmtStr, i);
      id = glGetUniformLocation(prog, id_name);
      texUnitId = startIndex + i;

      glUniform1i(id, texUnitId);
      light->shadow->bind_to_unit(texUnitId); 

      snprintf(id_name, 100, shadowMatFmtStr, i);
      id = glGetUniformLocation(prog, id_name);
      glUniform4fv(id, 1, glm::value_ptr(light->shadowMat())); 
    }
  } 
}
