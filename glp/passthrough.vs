#version 330 core
#define MAX_NUM_LIGHTS 100

out vec3 vColor;
/* All in world coordinates */
out vec3 vNormal;
out vec3 vEye;
out vec3 vHalf[MAX_NUM_LIGHTS];
out vec3 vLights[MAX_NUM_LIGHTS];

layout(location = 0) in vec3 in_Position;
layout(location = 1) in vec3 in_Normal;

uniform mat4 M_per;
uniform mat4 M_cam;
uniform vec3 eye;
uniform int num_lights;
uniform vec3 lights[MAX_NUM_LIGHTS];

void main(void) {
  /*
   * Hardware will do barycentric interpolation on
   * these values (after rasterization - e.g. clipping), 
   * then pass to fragment shader.
   */
  vColor = vec3(0.5,0.5,0.5);
  vNormal = in_Normal;
  
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
  for (i=0; i<num_lights; i++) {
    vec3 l = normalize(lights[i]-pos);
    vec3 h = normalize(v+l);
    vLights[i] = l;
    vHalf[i] = h;
  }
   
  vEye = v;

  // Calculate screen coordinates for input vertex
  vec4 _pos = M_per*M_cam*vec4(pos, 1.0);
  float w = _pos[3];
  gl_Position = _pos;
}
