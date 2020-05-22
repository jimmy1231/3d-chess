#version 430 core

in vec4 gl_FragCoord;

layout(location = 0) out vec4 out_Color;
void main(void) {
  out_Color = gl_FragCoord;
}
