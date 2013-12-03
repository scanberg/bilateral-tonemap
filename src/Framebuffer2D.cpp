#include "Framebuffer2D.h"
#include <cstdio>

void Framebuffer2D::bindDefault()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glDrawBuffer(GL_BACK);
}

Framebuffer2D::Framebuffer2D()
{
	glGenFramebuffers(1, &m_fboID);
}

Framebuffer2D::~Framebuffer2D()
{
	if(glIsFramebuffer(m_fboID))
		glDeleteFramebuffers(1, &m_fboID);
}

void Framebuffer2D::bind() const
{
	glBindFramebuffer(GL_FRAMEBUFFER, m_fboID);

}

void Framebuffer2D::attachTexture(GLenum attachment, const Texture2D & texture, GLint level)
{
	bind();
	glFramebufferTexture2D( GL_FRAMEBUFFER,
							attachment,
							GL_TEXTURE_2D,
							texture.getID(),
							level);
	bindDefault();
}