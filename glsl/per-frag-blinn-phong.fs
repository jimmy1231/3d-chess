#version 330 core
#define MAX_NUM_LIGHTS 4 

// Interpolated color - based on vColor in the vertex shader (from GPU rasterizer)
in vec3 vColor;
in vec3 vNormal;
in vec3 vEye;
in vec3 vHalf[MAX_NUM_LIGHTS];
in vec3 vLights[MAX_NUM_LIGHTS];

uniform vec3 Ia;
uniform vec3 ka;
uniform vec3 kd;
uniform vec3 ks;
uniform float p;
uniform int num_lights;
uniform vec3 intensity[MAX_NUM_LIGHTS];

layout(location = 0) out vec4 out_Fragmentcolor;

vec3 maxf3(vec3 v1, vec3 v2) {
  return vec3(
    max(v1.x, v2.x),
    max(v1.y, v2.y),
    max(v1.z, v2.z)
  );
}

vec3 minf3(vec3 v1, vec3 v2) {
  return vec3(
    min(v1.x, v2.x),
    min(v1.y, v2.y),
    min(v1.z, v2.z)
  );
}

vec3 powf3(vec3 v, float p) {
  return vec3(
    pow(v.x, p),
    pow(v.y, p),
    pow(v.z, p)
  );
}

void main(void) {
  /* 
   * L = Lambertian
   * S = Specular
   * A = Ambience
   */
  vec3 L;
  vec3 S;
  vec3 A;

  vec3 c = vec3(0,0,0);
  vec3 l, n, v, h;
  int i;
  int bound = min(num_lights, MAX_NUM_LIGHTS);
  for (i=0; i<bound; i++) {
    n = normalize(vNormal);
    v = normalize(vEye);
    h = normalize(vHalf[i]);
    l = normalize(vLights[i]);

    L = kd * intensity[i] * max(0, dot(n, l)); 
    S = ks * intensity[i] * pow(max(0, dot(n, h)), p); 

    c += (L+S);
  }

  A = ka * Ia;
  c = minf3(vec3(1,1,1), A+c);
  out_Fragmentcolor = vec4(c.xyz, 1.0);
}
