#ifndef __RENDER_SCENE_H__
#define __RENDER_SCENE_H__

#include <string>
#include <sstream>
#include <fstream>
#include <nlohmann/json.hpp>
#include <glm/geometric.hpp>
#include <glm/glm.hpp>

#include "helpers.h"

using json = nlohmann::json;

glm::vec3 parse_vec3(std::string str) {
  std::vector<float> parts;
  std::string _s = str;
  int i=0, j=0;
  while (i < _s.length()) {
    j = _s.find(" ", i);
    if (j == std::string::npos) {
      j = _s.length();
    }

    std::string substr = _s.substr(i, j-i);
    float num = std::stof(substr);
    parts.push_back(num);

    i = j+1;
  }

  assert(parts.size() >= 3);
  glm::vec3 vec(parts[0], parts[1], parts[2]);
  return vec;
}

class Light {
  public:
    glm::vec3 position;
    glm::vec3 intensity;

    Light(std::string p, std::string i) 
      : position(parse_vec3(p))
      , intensity(parse_vec3(i)) {}
};

class Orientation {
  public:
    glm::mat4 view_;
    glm::mat4 per_;

    glm::vec3 eye;
    glm::vec3 gaze;
    glm::vec3 top;

    float fovy;
    float zNear;
    float zFar;

    Orientation() {}
    Orientation(std::string e, std::string g, std::string t,
                float f, float n, float a) 
      : eye(parse_vec3(e))
      , gaze(parse_vec3(g))
      , top(parse_vec3(t))
      , fovy(f)
      , zNear(n)
      , zFar(a)
    {
      gaze = glm::normalize(gaze);
    }

    const GLfloat *view() {
      // Compute the camera basis
      glm::vec3 w, u, v;
      w = -glm::normalize(gaze);
      u = glm::normalize(glm::cross(top, w));
      v = glm::cross(w, u);

      // M_cam = [ u v w e ]^-1, where u, v, w are
      // vec4 with the 4th index set to 0.
      view_ = glm::inverse(glm::mat4(
        glm::vec4(u,0),
        glm::vec4(v,0),
        glm::vec4(w,0),
        glm::vec4(eye,1)
      ));

      return (const GLfloat *)&view_;
    }

    const GLfloat *perspective(const float &width, const float &height) {
      per_ = glm::perspective(
        glm::radians(fovy), width/height,
        zNear, zFar  
      );
      return (const GLfloat *)&per_;
    }
};

class Model {
  public:
    glm::mat4 model_; 

    Data *data;
    float rotationDeg;
    glm::vec3 rotationAxis;
    glm::vec3 scale;
    glm::vec3 translate;

    Model(Data *d, float r, std::string a, std::string s, std::string t)
      : data(d)
      , rotationDeg(r)
      , rotationAxis(parse_vec3(a))
      , scale(parse_vec3(s))
      , translate(parse_vec3(t)) {}

    const GLfloat *model() {
      model_ = glm::mat4(1.0f);
      return (const GLfloat *)&model_;       
    }
};

class Scene {
  public:
    std::string filename;

    std::vector<Light> lights_;
    Orientation orient;
    glm::vec3 Ia_;
    glm::vec3 Kd_;
    glm::vec3 Ks_;
    glm::vec3 Ka_;
    int p;

    std::unordered_map<std::string, Data *> objects;
    std::vector<Model> models;

    Scene() {}
    Scene(std::string n) : filename(n) {
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
            lightJson["intensity"].get<std::string>(),
            lightJson["position"].get<std::string>()
          );
          this->lights_.push_back(light); 
        }
      }

      // Miscellaneous
      {
        this->orient = {
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

      // Models
      {
        auto models = j["models"];
        assert(models.is_array());

        json modelJson;
        int i;
        for (i=0; i<models.size(); i++) {
          std::string object_id;

          modelJson = models[i];
          object_id = modelJson["object_id"].get<std::string>();
          assert(this->objects.count(object_id) > 0);
          Model model(
            this->objects[object_id],
            modelJson["rotation_deg"].get<float>(),
            modelJson["rotation_axis"].get<std::string>(),
            modelJson["scale"].get<std::string>(),
            modelJson["translate"].get<std::string>()
          );
          this->models.push_back(model);
        }
      }
    }
    const GLfloat *lights() {
      Light *data = this->lights_.data();
      return (const GLfloat *)data; 
    }
    const GLfloat *Kd() {return (const GLfloat *)&Kd_; }
    const GLfloat *Ka() {return (const GLfloat *)&Ka_; }
    const GLfloat *Ks() {return (const GLfloat *)&Ks_; }
    const GLfloat *Ia() {return (const GLfloat *)&Ia_; }
};

#endif /* __RENDER_SCENE_H__ */