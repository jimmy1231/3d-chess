#version 330 core

in float dist;

layout(location = 0) out vec4 out_Color;
void main(void) {
  float trunc_dist = min(255, dist);
  out_Color = vec4(0.5, 0.5, 0.5, 1.0);
}
