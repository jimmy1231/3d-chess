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

// Interpolated color - based on vColor in the vertex shader (from GPU rasterizer)
in float dist;
in vec3 vColor;
in vec3 vNormal;
in vec2 vTex; 
in vec3 vEye;
in VSLight vLights[MAX_NUM_LIGHTS];
in vec4 shadow_coords;

uniform vec3 Ia;
uniform vec3 ka;
uniform vec3 kd;
uniform vec3 ks;
uniform float p;
uniform int num_lights;
uniform Light lights[MAX_NUM_LIGHTS];
uniform sampler2D tex;
uniform sampler2DShadow shadow_tex;

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

  /*
   * textureProj will take as input a vec4 := (x,y,z,w),
   * apply perspective division, then take the x-y 
   * components as u,v coordinates to match against the
   * texture. 
   * In addition, it performs a depth (z) buffer check:
   *    If the depth (z) value of the input vec4 coord
   *    is GREATER THAN value of the texture, then
   *    texture returns (0,0,0) as color - since it is
   *    hidden in shadow
   */
  vec3 shadowTexel = (textureProj(shadow_tex, shadow_coords) * vec4(1.0)).rgb;
  vec3 kdTexel = texture(tex, vTex).rgb;
  vec3 c = vec3(0,0,0);
  vec3 l, n, v, h;
  vec3 intensity;
  int i;
  int bound = min(num_lights, MAX_NUM_LIGHTS);

  float e = 2.0;
  if (true || dist <= shadowTexel.x + e) {
    for (i=0; i<bound; i++) {
      n = normalize(vNormal);
      v = normalize(vEye);
      h = normalize(vLights[i].h);
      l = normalize(vLights[i].l);
      intensity = lights[i].intensity;

      L = kdTexel * shadowTexel *intensity * max(0, dot(n, l)); 
      S = ks * intensity * pow(max(0, dot(n, h)), p); 

      c += (L+S);
    }
  }

  A = ka * Ia;
  c = minf3(vec3(1,1,1), A+c);
  out_Fragmentcolor = vec4(c.xyz, 1.0);
}
