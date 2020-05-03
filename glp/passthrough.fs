#version 330 core

in vec3 vColor;

layout(location = 0) out vec4 out_Fragmentcolor;
void main(void) {
	out_Fragmentcolor = vec4(vColor, 1.0);
}
