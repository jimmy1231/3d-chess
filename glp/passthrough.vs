#version 330 core

out vec3 vColor;

layout(location = 0) in vec3 in_Position;
layout(location = 1) in vec3 in_Color;

uniform mat4 M_per;
uniform mat4 M_cam;

void main(void) {
  vColor = in_Color;

  // Calculate screen coordinates for input vertex
  vec4 _pos = M_per*M_cam*vec4(in_Position, 1.0);
  float w = _pos[3];
	// gl_Position = vec4(_pos.x/w, _pos.y/w, _pos.z/w, 1.0);
  // gl_Position = vec4(in_Position, 1.0);
  gl_Position = _pos;
}
