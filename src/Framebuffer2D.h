#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "Texture2D.h"

#ifndef NULL
#define NULL 0
#endif

class Framebuffer2D
{
public:
	static void bindDefault();

	Framebuffer2D();
	~Framebuffer2D();

	void bind() const;

	void attachTexture(GLenum attachment, const Texture2D & texture, GLint level=0);
private:
	GLuint	m_fboID;
};