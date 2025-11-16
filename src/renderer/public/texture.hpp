#pragma once
#include <string>
#include <vector>

class ITexture
{
    public:
        virtual ~ITexture() = default;
        virtual void Bind() = 0;
        virtual void UnBind() = 0;
        virtual unsigned int GetID() const = 0;
};

class OpenGLTexture:public ITexture
{
    public:
        OpenGLTexture(const std::string& path);
        OpenGLTexture(const unsigned char* data, unsigned int size);//png,jpg
        OpenGLTexture(const unsigned char* data, unsigned int width, unsigned int height, int channel = 0);//row
        ~OpenGLTexture();
        void Bind() override;
        void UnBind() override;
        unsigned int GetID() const override {return textureID;}
    private:
        unsigned int textureID;
        int width, height, nrChannels;
};

class OpenGLCubeTexture:public ITexture
{
    public:
        OpenGLCubeTexture(std::vector<std::string> paths);
        ~OpenGLCubeTexture();
        void Bind() override;
        void UnBind() override;
        unsigned int GetID()const override {return imageID;}
    private:
        void CreateImageTexture();
        unsigned int textureID, imageID;
        int width, height, nrChannels;
};