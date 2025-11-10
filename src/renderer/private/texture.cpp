#include "texture.hpp"
#include "glad/glad.h"

OpenGLTexture::OpenGLTexture(const std::string& path)
{
    glGenTextures(1, &textureID);
}

OpenGLTexture::~OpenGLTexture()
{
    glDeleteTextures(1, &textureID);   
}

void OpenGLTexture::Bind()
{
    glBindTexture(GL_TEXTURE_2D, textureID);
}

void OpenGLTexture::UnBind()
{
    glBindTexture(GL_TEXTURE_2D, 0);
}