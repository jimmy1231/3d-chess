#version 330 core
#define MAX_NUM_LIGHTS 4 

struct Light {
  vec3 position;
  vec3 intensity;  
};

struct VSLight {
  vec3 l;
  vec3 h;
};

out vec3 vColor;
/* All in world coordinates */
out float dist;
out vec3 vNormal;
out vec2 vTex;
out vec3 vEye;
out VSLight vLights[MAX_NUM_LIGHTS];
out vec4 shadow_coords;

layout(location = 0) in vec3 in_Position;
layout(location = 1) in vec3 in_Normal;
layout(location = 2) in vec2 in_Tex;

uniform mat4 M_model;
uniform mat4 M_per;
uniform mat4 M_cam;
uniform mat4 M_light;
uniform mat4 M_scale_bias;
uniform vec3 eye;
uniform int num_lights;
uniform Light lights[MAX_NUM_LIGHTS];

void main(void) {
  /*
   * Hardware will do barycentric interpolation on
   * these values (after rasterization - e.g. clipping), 
   * then pass to fragment shader.
   */
  vColor = vec3(0.5,0.5,0.5);
  vNormal = in_Normal;
  vTex = in_Tex;
  
  /*
   * Specify v, l, h vectors per-vertex as output of the
   * vertex processing stage. Rasterizer will then interpolate
   * these values (hardware) and pass the interpolated
   * values of v, l, h for each fragment of the primitive
   * (e.g. GL_TRIANGLE) to the fragment shader.
   * This way, we can perform per-fragment shading.
   *
   * We could do shading here in the vertex-processing stage.
   * This is known as per-vertex shading, but per-fragment
   * shading yields far better shading results as shading is
   * done per-fragment (i.e. more fine-grained), than per-vertex,
   * which essentially tells the GPU to "guess" (interpolate)
   * the color values.
   */
  vec3 pos = in_Position.xyz;
  vec3 v = normalize(eye-pos);

  vec3 l, h;
  int i;
  int bound = min(num_lights, MAX_NUM_LIGHTS);
  for (i=0; i<bound; i++) {
    vec3 l = normalize(lights[i].position-pos);
    vec3 h = normalize(v+l);
    vLights[i].l = l;
    vLights[i].h = h;
  }
   
  vEye = v;

  // Calculate screen coordinates for input vertex
  vec4 _pos = M_per * M_cam * M_model * vec4(pos, 1.0);
  shadow_coords = M_scale_bias * M_per * M_light * M_model * vec4(pos, 1.0);
  gl_Position = _pos;
}
