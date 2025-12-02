#pragma once
#include <memory>
#include <vector>
#include <glm/glm.hpp>

class IMesh;
class IShader;

enum PostEffectType
{
    None = 0,
    OnMinus,
    Gray,
    Kernel,
    Blur,
    Edge
};

class IRenderer
{
    public:
        virtual ~IRenderer() = default;
        virtual void Clear() = 0;
        virtual void Begin() = 0;
        virtual void Draw() = 0;
        virtual void Update() = 0;
        virtual unsigned int& GetColorBuffer() = 0;
        virtual unsigned int& GetShadowBuffer() = 0;
        virtual unsigned int& GetGBufferPosition() = 0;
        virtual unsigned int& GetGBufferNormal() = 0;
        virtual unsigned int& GetGBufferDiffuse() = 0;
        virtual unsigned int& GetGBufferRoughness() = 0;
        virtual unsigned int& GetGBufferMetallic() = 0;
        virtual unsigned int& GetGBufferOcclusion() = 0;
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
        unsigned int& GetColorBuffer() override{return pcbo;}
        unsigned int& GetShadowBuffer() override{return shadowFrameTexture;}
        unsigned int& GetGBufferPosition() override{return gpos;}
        unsigned int& GetGBufferNormal() override{return gnor;}
        unsigned int& GetGBufferDiffuse() override{return gdiff;}
        unsigned int& GetGBufferRoughness() override{return texViewRoughness;}
        unsigned int& GetGBufferMetallic() override{return texViewMetallic;}
        unsigned int& GetGBufferOcclusion() override{return texViewOcclusion;}
        float* GetSSAORadius(){return &ssaoRadius;}
        float* GetSSAOBias(){return &ssaobias;}
        void ResizeBuffer(int w, int h);
        float GetAspect()const{return float(width)/height;}
        bool* GetStencil(){return &bStencil;}
        float* GetExposure(){return &exposure;}
        int* GetDeferred(){return &bDeferred;}
        void SetEffectType(PostEffectType type){postEffect = type;}

    private:
        void CreateBuffer(int width, int height);
        void CreateGBuffer(int width, int height);
        void CreateSSAOBuffer(int width, int height);
        unsigned int vubo;//shader uniform
        unsigned int fbo, rbo, cbo;// forward renderbuffer
        unsigned int shadowFrameBuffer, shadowFrameTexture;//shadow map
        unsigned int gfbo, gdepth;//geometry framebuffer
        unsigned int gpos, gnor, gdiff;//geometry colorbuffer
        unsigned int gORMI;//geometry colorbuffer(AO, Roughness, Metallic, Displacement)
        unsigned int texViewOcclusion, texViewRoughness, texViewMetallic, texViewID;//view
        unsigned int debugTexture;//debug texture
        unsigned int ssaofbo, ssaocbo, ssaoblurfbo, ssaoblurcbo;//ssao, ssao blur buffer
        unsigned int dfbo, dcbo; //deferred framebuffer
        unsigned int pfbo, pcbo;//posteffect
        unsigned int finalColor; //finalColor
        int width, height;
        bool bStencil = false;
        int bDeferred = 1;
        std::unique_ptr<IMesh> rendererMesh;
        std::unique_ptr<IShader> rendererShader;
        PostEffectType postEffect = PostEffectType::None;
        float exposure = 2.2f;
        //occlusion
        std::vector<glm::vec3>ssaoKernel;
        std::vector<glm::vec3> ssaoNoise;
        unsigned int noiseTexture;
        int kernelSize = 64;
        float ssaoRadius = 0.5f;
        float ssaobias = 0.025f;
};