#ifndef __RENDER_LIGHT_H__
#define __RENDER_LIGHT_H__

#include <glm/glm.hpp>
#include "helpers.h"
#include "Texture.h"

class Light {
  public:
    static const size_t Size = 6;
    Texture *shadow = NULL;
    glm::vec3 position;
    glm::vec3 intensity;

    Light(std::string p, std::string i) 
      : position(parse_vec3(p))
      , intensity(parse_vec3(i)) {}

    void initShadow(const GLuint prog, const Scene &&scene) {
      shadow = new Texture(NULL, scene.WIDTH, scene.HEIGHT);

      glm::vec3 gaze = glm::vec3(0,0,0) - position;
      shadow->shadow_map(prog, scene,
                         gaze, scene.orient.top, position);
    }
};

#endif /* __RENDER_LIGHT_H__ */