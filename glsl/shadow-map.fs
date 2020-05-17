#version 330 core

in float d;
in vec4 gl_FragCoord;

layout(location = 0) out vec4 out_Color;
void main(void) {
  float _d = gl_FragCoord.z;
  out_Color = vec4(_d,_d,_d,1.0);
}
