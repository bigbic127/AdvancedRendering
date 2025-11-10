#pragma once
#include <string>

class ITexture
{
    public:
        virtual ~ITexture() = default;
        virtual void Bind() = 0;
        virtual void UnBind() = 0;
};

class OpenGLTexture:public ITexture
{
    public:
        OpenGLTexture(const std::string& path);
        OpenGLTexture(const unsigned char* data, unsigned int size);//png,jpg
        OpenGLTexture(const unsigned char* data, unsigned int width, unsigned int height);//row
        ~OpenGLTexture();
        void Bind() override;
        void UnBind() override;
    private:
        unsigned int textureID;
        int width, height, nrChannels;
};