#ifndef __RENDER_SCENE_H__
#define __RENDER_SCENE_H__

#include <string>
#include <sstream>
#include <fstream>
#include <nlohmann/json.hpp>
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
      : position(parse_vec3(p)), intensity(parse_vec3(i)) {}
};

class Orientation {
  public:
    glm::vec3 eye;
    glm::vec3 gaze;
    glm::vec3 top;
};

class Model {
  public:
    std::string objectId;
    float rotationDeg;
    glm::vec3 rotationAxis;
    glm::vec3 scale;
    glm::vec3 translate;
};

class Scene {
  public:
    std::string filename;

    std::vector<Light> lights;
    Orientation orient;
    glm::vec3 Ia;
    glm::vec3 Kd;
    glm::vec3 Ks;
    glm::vec3 Ka;
    int p;

    std::unordered_map<std::string, Data> objects;
    std::vector<Model> models;

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
          this->lights.push_back(light); 
        }
      }

      // Miscellaneous
      {
        top = parse_vec3(j["top"].get<std::string>()); 
        eye = parse_vec3(j["eye"].get<std::string>()); 
        gaze = parse_vec3(j["gaze"].get<std::string>()); 
        Ia = parse_vec3(j["Ia"].get<std::string>()); 
        Kd = parse_vec3(j["Kd"].get<std::string>()); 
        Ka = parse_vec3(j["Ka"].get<std::string>()); 
        Ks = parse_vec3(j["Ks"].get<std::string>()); 
        p = j["p"].get<int>();
      }

      // Objects
      {
        auto objects = j["objects"];
        assert(objects.is_array());

        std::string id, filename;
        json objectJson;
        int i;
        for (i=0; i<objects.size(); i++) {
          objectJson = objects[i];
          id = objectJson["id"].get<std::string>();
          filename = objectJson["filename"].get<std::string>();

          this->objects[id] = new Data(filename); 
        }
      }
    }
};

#endif /* __RENDER_SCENE_H__ */