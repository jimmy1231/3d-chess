#version 330 core

in float d;
in vec4 gl_FragCoord;

layout(location = 0) out vec4 out_Color;
void main(void) {
  float _d = gl_FragCoord.z;
  if (_d > 1.0) {
    out_Color = vec4(0.2, 0.3, 0.4, 1.0);
  } else {
    out_Color = vec4(d,d,d,1.0);
  }
}
