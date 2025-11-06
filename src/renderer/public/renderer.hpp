#pragma once

class IRenderer
{
    public:
        virtual ~IRenderer() = default;
        virtual void Clear() = 0;
        virtual void Begin() = 0;
        virtual void Draw() = 0;
        virtual void End() = 0;
        virtual unsigned int& GetColorBuffer() = 0;
};

class OpenGLRenderer:public IRenderer
{
    public:
        OpenGLRenderer();
        ~OpenGLRenderer();
        void Clear() override;
        void Begin() override;
        void Draw() override;
        void End() override;
        unsigned int& GetColorBuffer() override{return cbo;}
    private:
        unsigned int fbo, rbo, cbo;

};