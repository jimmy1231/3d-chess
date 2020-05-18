#version 330 core

uniform mat4 M_per;
uniform mat4 M_cam;
uniform mat4 M_model;

layout(location = 0) in vec3 in_Position;

void main(void) {
  vec4 pos = M_per * M_cam * M_model * vec4(in_Position, 1.0);
  gl_Position = pos;
}
