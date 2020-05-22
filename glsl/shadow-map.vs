#version 430 core

layout(location = 0) in vec3 in_Position;
layout(location = 1) uniform mat4 shadowMat;
layout(location = 2) uniform mat4 model;

void main(void) {
  vec4 pos = shadowMat * model * vec4(in_Position, 1.0);
  gl_Position = pos;
}
