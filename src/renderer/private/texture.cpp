#include "renderer/public/texture.hpp"
#include <glad/glad.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include "folderPath.hpp"
#include "logger.hpp"

OpenGLTexture::OpenGLTexture(const std::string& path)
{
    std::string p = path;
    std::string root = GetExecutableDir();
    p = root + path;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    stbi_set_flip_vertically_on_load(true);
    unsigned char* data = stbi_load(p.c_str(), &width, &height, &nrChannels, 0);
    if(data)
    {
        GLenum format;
        switch (nrChannels)
        {
            case 1:format = GL_RED;break;
            case 2:format = GL_RG;break;
            case 3:format = GL_RGB;break;
            case 4:format = GL_RGBA;
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            break;
            default:stbi_image_free(data);
        }
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
        stbi_image_free(data);
    }
    glBindTexture(GL_TEXTURE_2D, 0);;
}

OpenGLTexture::OpenGLTexture(const unsigned char* data, unsigned int size)
{
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    stbi_set_flip_vertically_on_load(true);
    unsigned char* _data = stbi_load_from_memory(data, size, &width, &height, &nrChannels, 0);
    if(_data)
    {
        GLenum format;
        switch (nrChannels)
        {
            case 1:format = GL_RED;break;
            case 2:format = GL_RG;break;
            case 3:format = GL_RGB;break;
            case 4:format = GL_RGBA;break;
            default:stbi_image_free(_data);
        }
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, _data);
        glGenerateMipmap(GL_TEXTURE_2D);
        stbi_image_free(_data);
    }
    glBindTexture(GL_TEXTURE_2D, 0);
}

OpenGLTexture::OpenGLTexture(const unsigned char* data, unsigned int width, unsigned int height, int channel)
{
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    GLenum format;
    switch(channel)
    {
        case 1:format = GL_RED; break;
        case 2:format = GL_RG; break;
        case 3:format = GL_RGB; break;
        case 4:format = GL_RGBA; break;
        default:format = GL_RGBA;
    }
    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
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

//CubeMap
OpenGLCubeTexture::OpenGLCubeTexture(std::vector<std::string> paths)
{
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    unsigned int positive = 0;
    std::string root = GetExecutableDir();
    for (auto& path : paths)
    {
        std::string p = root + path;
        stbi_set_flip_vertically_on_load(false);
        unsigned char* data = stbi_load(p.c_str(), &width, &height, &nrChannels, 0);
        if(data)
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + positive, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        else
            Logger::ErrorMessage("Cubemap texture failed to load.");
        stbi_image_free(data);
        positive += 1;
    }
    CreateImageTexture();
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}

OpenGLCubeTexture::~OpenGLCubeTexture()
{
    glDeleteTextures(1, &textureID);
    glDeleteTextures(1, &imageID);
}

void OpenGLCubeTexture::CreateImageTexture()
{
    int w = width*3;
    int h = height*2;
    glGenTextures(1, &imageID);
    glBindTexture(GL_TEXTURE_2D, imageID);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);

    std::vector<unsigned char> data(width * height * 3);
    int x=0, y=1;
    for (int i = 0; i < 6; ++i)
    {
        glGetTexImage(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, GL_UNSIGNED_BYTE, data.data());
        glBindTexture(GL_TEXTURE_2D, imageID);
        glTexSubImage2D(GL_TEXTURE_2D, 0, x*width, y*height, width, height, GL_RGB, GL_UNSIGNED_BYTE, data.data());
        x++;
        if(i==2)
        {
            x=0;
            y=0;
        }
    }
    glBindTexture(GL_TEXTURE_2D, 0);
}

void OpenGLCubeTexture::Bind()
{
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
}

void OpenGLCubeTexture::UnBind()
{
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}