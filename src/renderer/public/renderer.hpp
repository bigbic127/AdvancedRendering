#pragma once

class IRenderer
{
    public:
        virtual ~IRenderer() = default;
        virtual void Clear() = 0;
        virtual void Draw() = 0;
        virtual void Update() = 0;
        virtual unsigned int& GetColorBuffer() = 0;
        virtual void CreateBuffer(int width, int height) = 0;
        virtual void ResizeBuffer(int width, int height) = 0;
};

class OpenGLRenderer:public IRenderer
{
    public:
        OpenGLRenderer();
        ~OpenGLRenderer();
        void Clear() override;
        void Draw() override;
        void Update() override;
        unsigned int& GetColorBuffer() override{return cbo;}
        void ResizeBuffer(int w, int h);
    private:
        void CreateBuffer(int width, int height);
        unsigned int fbo, rbo, cbo;

};