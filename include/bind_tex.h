#ifndef __RENDER_BIND_TEX_H__
#define __RENDER_BIND_TEX_H__

#include <glad/glad.h>

void bind_tex(const unsigned char *data,
						  const int width,
						  const int height,
						  GLuint &TEX) {
	GLuint _TEX;
	glGenTextures(1, &_TEX);
	glBindTexture(GL_TEXTURE_2D, _TEX);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 
		height, width, 0, 
		GL_RGB, GL_UNSIGNED_BYTE, data);

	glBindTexture(GL_TEXTURE_2D, 0);
	TEX = _TEX;
	printf("Initialized texture\n");
}

#endif