#pragma once
#include <memory>

class IMesh;
class IShader;

class IRenderer
{
    public:
        virtual ~IRenderer() = default;
        virtual void Clear() = 0;
        virtual void Begin() = 0;
        virtual void Draw() = 0;
        virtual void Update() = 0;
        virtual unsigned int& GetColorBuffer() = 0;
        virtual void CreateBuffer(int width, int height) = 0;
        virtual void ResizeBuffer(int width, int height) = 0;
        virtual float GetAspect()const = 0;
};

class OpenGLRenderer:public IRenderer
{
    public:
        OpenGLRenderer();
        ~OpenGLRenderer();
        void Clear() override;
        void Begin() override;
        void Draw() override;
        void Update() override;
        unsigned int& GetColorBuffer() override{return cbo2;}
        void ResizeBuffer(int w, int h);
        float GetAspect()const{return float(width)/height;}
        bool* GetStencil(){return &bStencil;}
    private:
        void CreateBuffer(int width, int height);
        unsigned int fbo, rbo, cbo, fbo2, cbo2;
        int width, height;
        bool bStencil = false;
        std::unique_ptr<IMesh> rendererMesh;
        std::unique_ptr<IShader> rendererShader;
};