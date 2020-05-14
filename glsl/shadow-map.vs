#version 330 core

out float dist;

uniform mat4 per;
uniform mat4 view;
uniform vec3 light;

layout(location = 0) in vec3 in_Position;

void main(void) {
  dist = length(in_Position - light); 
  gl_Position = per * view * vec4(in_Position, 1.0);	
}
