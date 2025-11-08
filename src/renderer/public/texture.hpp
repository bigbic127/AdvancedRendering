#pragma once
#include <string>

class ITexture
{
    public:
        virtual ~ITexture() = default;
};

class OpenGLTexture:public ITexture
{
    public:
        OpenGLTexture(const std::string& path);
        OpenGLTexture(const unsigned char* data, unsigned int size);//png,jpg
        OpenGLTexture(const unsigned char* data, unsigned int width, unsigned int height);//row
        ~OpenGLTexture();
    private:
        unsigned int textureID;
        int width, height, nrChannels;
};