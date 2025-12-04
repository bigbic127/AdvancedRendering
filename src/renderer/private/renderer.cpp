#include "renderer.hpp"
#include "glad/glad.h"
#include "context.hpp"
#include "world.hpp"
#include "resourceManager.hpp"
#include "material.hpp"
#include "meshComponent.hpp"
#include "cameraComponent.hpp"
#include "lightComponent.hpp"
#include "logger.hpp"
#include "shader.hpp"
#include <random>

OpenGLRenderer::OpenGLRenderer()
{
    //create mesh and shader
    rendererMesh = std::make_unique<QaudMesh>();
    const std::string vsPath = "/shader/posteffect.vert";
    const std::string fsPath = "/shader/posteffect.frag";
    rendererShader = std::make_unique<OpenGLShader>(vsPath, fsPath);
    //static_cast<OpenGLShader*>(rendererShader.get())->SetInt("screenTexture", 10);
    //glActiveTexture(GL_TEXTURE10);
    //opengl setting
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glEnable(GL_STENCIL_TEST);
    glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_MULTISAMPLE);
    CreateBuffer(800, 600);
    //create ssao
    std::uniform_real_distribution<GLfloat> randomFloats(0.0f, 1.0f);
    std::default_random_engine generator;
    for(size_t i = 0;i<64;++i)
    {
        glm::vec3 sample(randomFloats(generator)*2.0-1.0, randomFloats(generator)*2.0-1.0, randomFloats(generator));
        sample = glm::normalize(sample);
        sample *= randomFloats(generator);
        float scale = float(i)/ 64.0f;
        scale = 0.1f + (scale*scale) * (1.0f - 0.1f);
        sample *= scale;
        ssaoKernel.push_back(sample);
    }
    //create noise texture
    for (size_t i = 0;i<16;i++)
    {
        glm::vec3 noise(randomFloats(generator)*2.0-1.0, randomFloats(generator)*2.0-1.0, 0.0f); // rotate around z-axis (in tangent space)
        ssaoNoise.push_back(noise);
    }
    glGenTextures(1, &noiseTexture);
    glBindTexture(GL_TEXTURE_2D, noiseTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, 4, 4, 0, GL_RGB, GL_FLOAT, &ssaoNoise[0]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
}

OpenGLRenderer::~OpenGLRenderer()
{
    glDeleteFramebuffers(1, &fbo);
    glDeleteRenderbuffers(1, &rbo);
    glDeleteTextures(1, &cbo);
    glDeleteFramebuffers(1, &pfbo);
    glDeleteTextures(1, &pcbo);
    glDeleteTextures(1, &shadowFrameBuffer);
    glDeleteTextures(1, &shadowFrameTexture);
    glDeleteFramebuffers(1, &gfbo);
    glDeleteTextures(1, &gpos);
    glDeleteTextures(1, &gnor);
    glDeleteTextures(1, &gdiff);
    glDeleteTextures(1, &gORMI);
    glDeleteTextures(1, &gdepth);
    glDeleteFramebuffers(1, &ssaofbo);
    glDeleteTextures(1, &ssaocbo);
    glDeleteFramebuffers(1, &ssaoblurfbo);
    glDeleteTextures(1, &ssaoblurcbo);
    glDeleteFramebuffers(1, &dfbo);
    glDeleteTextures(1, &dcbo);
    glDeleteBuffers(1, &vubo);
    glDeleteTextures(1, &texViewOcclusion);
    glDeleteTextures(1, &texViewRoughness);
    glDeleteTextures(1, &texViewMetallic);
}

void OpenGLRenderer::CreateBuffer(int width, int height)
{
    this->width = width;
    this->height = height;
    //forward framebuffer
    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    //colorTexture
    glGenTextures(1, &cbo);
    glBindTexture(GL_TEXTURE_2D, cbo);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, 4, GL_RGBA16F, width, height, GL_TRUE);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, cbo, 0);
    //depth, stencill buffer
    glGenRenderbuffers(1, &rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    glRenderbufferStorageMultisample(GL_RENDERBUFFER, 4, GL_DEPTH24_STENCIL8, width, height);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);
    //
    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        Logger::ErrorMessage("Framebuffer is not complete!");
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    //post framebuffer
    glGenFramebuffers(1, &pfbo);
    glBindFramebuffer(GL_FRAMEBUFFER, pfbo);
    //colorTexture
    glGenTextures(1, &pcbo);
    glBindTexture(GL_TEXTURE_2D, pcbo);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, 4, GL_RGBA8, width, height, GL_TRUE);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, pcbo, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    //debugTexture
    glGenTextures(1, &texViewOcclusion);
    glBindTexture(GL_TEXTURE_2D, texViewOcclusion);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    GLint swizzleR[] = { GL_RED, GL_RED, GL_RED, GL_ONE };
    glTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_RGBA, swizzleR);
    glGenTextures(1, &texViewRoughness);
    glBindTexture(GL_TEXTURE_2D, texViewRoughness);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    GLint swizzleG[] = { GL_GREEN, GL_GREEN, GL_GREEN, GL_ONE };
    glTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_RGBA, swizzleG);
    glGenTextures(1, &texViewMetallic);
    glBindTexture(GL_TEXTURE_2D, texViewMetallic);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    GLint swizzleB[] = { GL_BLUE, GL_BLUE, GL_BLUE, GL_ONE };
    glTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_RGBA, swizzleB);
    glBindTexture(GL_TEXTURE_2D, 0);

    CreateGBuffer(width, height);
    CreateSSAOBuffer(width, height);
    glViewport(0, 0, width, height);
}

void OpenGLRenderer::CreateGBuffer(int width, int height)
{
    glGenFramebuffers(1, &gfbo);
    glBindFramebuffer(GL_FRAMEBUFFER, gfbo);
    //position
    glGenTextures(1, &gpos);
    glBindTexture(GL_TEXTURE_2D, gpos);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gpos, 0);
    //normal
    glGenTextures(1, &gnor);
    glBindTexture(GL_TEXTURE_2D, gnor);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, gnor, 0);    
    //diffuse
    glGenTextures(1, &gdiff);
    glBindTexture(GL_TEXTURE_2D, gdiff);
    glTexImage2D(GL_TEXTURE_2D, 0 , GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, gdiff, 0);
    //ORMD(Occlusion, Roughness, Metallic, MaterialID)
    glGenTextures(1, &gORMI);
    glBindTexture(GL_TEXTURE_2D, gORMI);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, GL_TEXTURE_2D, gORMI, 0);
    //
    unsigned int attachments[4] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3};
    glDrawBuffers(4, attachments);
    //depth
    glGenTextures(1, &gdepth);
    glBindTexture(GL_TEXTURE_2D, gdepth);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, width, height, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, gdepth, 0);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "Framebuffer not complete!" << std::endl;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    //deferred framebuffer
    glGenFramebuffers(1, &dfbo);
    glBindFramebuffer(GL_FRAMEBUFFER, dfbo);
    //colorTexture
    glGenTextures(1, &dcbo);
    glBindTexture(GL_TEXTURE_2D, dcbo);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    //glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, 4, GL_RGBA8, width, height, GL_TRUE);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, dcbo, 0);
    //depth
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, gdepth, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void OpenGLRenderer::CreateSSAOBuffer(int width, int height)
{
    //ssao buffer
    glGenFramebuffers(1, &ssaofbo);
    glBindFramebuffer(GL_FRAMEBUFFER, ssaofbo);
    //texture
    glGenTextures(1, &ssaocbo);
    glBindTexture(GL_TEXTURE_2D, ssaocbo);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, width, height, 0, GL_RED, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ssaocbo, 0);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            std::cout << "SSAO Framebuffer not complete!" << std::endl;    
    //ssao blur buffer
    glGenFramebuffers(1, &ssaoblurfbo);
    glBindFramebuffer(GL_FRAMEBUFFER, ssaoblurfbo);
    //texture
    glGenTextures(1, &ssaoblurcbo);
    glBindTexture(GL_TEXTURE_2D, ssaoblurcbo);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, width, height, 0, GL_RGB, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ssaoblurcbo, 0);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "SSAO Blur Framebuffer not complete!" << std::endl;
    glGenFramebuffers(GL_FRAMEBUFFER, 0);
}


void OpenGLRenderer::ResizeBuffer(int width, int height)
{
    if(width == this->width && height == this->height) return;
    glDeleteFramebuffers(1, &fbo);
    glDeleteRenderbuffers(1, &rbo);
    glDeleteTextures(1, &cbo);
    glDeleteFramebuffers(1, &pfbo);
    glDeleteTextures(1, &pcbo);
    glDeleteFramebuffers(1, &gfbo);
    glDeleteTextures(1, &gpos);
    glDeleteTextures(1, &gnor);
    glDeleteTextures(1, &gdiff);
    glDeleteTextures(1, &gORMI);
    glDeleteTextures(1, &gdepth);
    glDeleteFramebuffers(1, &ssaofbo);
    glDeleteTextures(1, &ssaocbo);
    glDeleteFramebuffers(1, &ssaoblurfbo);
    glDeleteTextures(1, &ssaoblurcbo);
    glDeleteFramebuffers(1, &dfbo);
    glDeleteTextures(1, &dcbo);
    glDeleteTextures(1, &texViewOcclusion);
    glDeleteTextures(1, &texViewRoughness);
    glDeleteTextures(1, &texViewMetallic);
    CreateBuffer(width, height);
}

void OpenGLRenderer::Clear()
{
    glClearColor(0.0f,0.0f,0.0f,1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
}

void OpenGLRenderer::Begin()
{
    //shadowMap framebuffer
    glGenFramebuffers(1, &shadowFrameBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, shadowFrameBuffer);
    //shadowMap texture
    unsigned int shadow_width = 4096, shadow_height = 4096;
    glGenTextures(1, &shadowFrameTexture);
    glBindTexture(GL_TEXTURE_2D, shadowFrameTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, shadow_width, shadow_height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER); 
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, shadowFrameTexture, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    //shader uniform
    //standard
    IShader* shader = Context::GetContext()->resourceManager->FindShader("standardShader");
    unsigned int shaderID = static_cast<OpenGLShader*>(shader)->GetID();
    unsigned int index = glGetUniformBlockIndex(shaderID, "mCamera");
    glUniformBlockBinding(shaderID, index, 0);
    //skybox
    shader = Context::GetContext()->resourceManager->FindShader("skyboxShader");
    shaderID = static_cast<OpenGLShader*>(shader)->GetID();
    index = glGetUniformBlockIndex(shaderID, "mCamera");
    glUniformBlockBinding(shaderID, index, 0); 
    //Shader Uniform buffers
    glGenBuffers(1, &vubo);
    glBindBuffer(GL_UNIFORM_BUFFER, vubo);
    glBufferData(GL_UNIFORM_BUFFER, 2*sizeof(glm::mat4), NULL, GL_STATIC_DRAW);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
    glBindBufferRange(GL_UNIFORM_BUFFER, 0, vubo, 0, 2*sizeof(glm::mat4));

    //Create HDRI to skyCubeMap
    unsigned int captureFBO, captureRBO;
    glGenFramebuffers(1, &captureFBO);

    glGenRenderbuffers(1, &captureRBO);
    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
    glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 512, 512);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, captureRBO);

    IMesh* skyboxMesh = Context::GetContext()->resourceManager->FindMesh("skybox");
    IShader* hdriShader = Context::GetContext()->resourceManager->FindShader("hdriShader");
    ITexture* hdriTexture = Context::GetContext()->resourceManager->FindTexture("HDR");

    glm::mat4 captureProjection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
    glm::mat4 captureViews[] = {
        glm::lookAt(glm::vec3(0.0f), glm::vec3( 1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)), // +X
        glm::lookAt(glm::vec3(0.0f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)), // -X
        glm::lookAt(glm::vec3(0.0f), glm::vec3( 0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f)), // +Y
        glm::lookAt(glm::vec3(0.0f), glm::vec3( 0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f)), // -Y
        glm::lookAt(glm::vec3(0.0f), glm::vec3( 0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f)), // +Z
        glm::lookAt(glm::vec3(0.0f), glm::vec3( 0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f))  // -Z
    };
    OpenGLShader* glHDRShader = static_cast<OpenGLShader*>(hdriShader);
    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
    glHDRShader->UseProgram();
    glHDRShader->SetInt("hdrTexture", 0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, hdriTexture->GetImageID());
    glViewport(0, 0, 512, 512);
    glDepthFunc(GL_LEQUAL);
    glCullFace(GL_FRONT);
    glHDRShader->SetMatrix4("mProjection", captureProjection);
    for(unsigned i=0;i<6;++i)
    {
        glHDRShader->SetMatrix4("mView", captureViews[i]);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                                GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, hdriTexture->GetID(), 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        skyboxMesh->Bind();
        skyboxMesh->Draw();
        skyboxMesh->UnBind();
    }
    glBindTexture(GL_TEXTURE_2D, 0);
    glHDRShader->EndProgam();
    glCullFace(GL_BACK);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    //illuminance
    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
    glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 32, 32);
    IShader* illuminanceShader = Context::GetContext()->resourceManager->FindShader("illuminanceShader");
    OpenGLShader* glIlluminanceShader = static_cast<OpenGLShader*>(illuminanceShader);
    OpenGLHDRTexture* illuminanceTexture = static_cast<OpenGLHDRTexture*>(hdriTexture);
    glIlluminanceShader->UseProgram();
    glIlluminanceShader->SetInt("environmentMap", 0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, hdriTexture->GetID());
    glViewport(0, 0, 32, 32);
    glDepthFunc(GL_LEQUAL);
    glCullFace(GL_FRONT);
    glIlluminanceShader->SetMatrix4("mProjection", captureProjection);
    for(unsigned i=0;i<6;++i)
    {
        glIlluminanceShader->SetMatrix4("mView", captureViews[i]);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                                GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, illuminanceTexture->GetIlluminance(), 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        skyboxMesh->Bind();
        skyboxMesh->Draw();
        skyboxMesh->UnBind();
    }
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
    glIlluminanceShader->EndProgam();
    glCullFace(GL_BACK);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glDepthFunc(GL_LESS);
}

void OpenGLRenderer::Draw()
{
    CameraComponent* cameraComponent = Context::GetContext()->world->GetCurrentCamera()->GetComponent<CameraComponent>();
    LightComponent* lightComponent = Context::GetContext()->world->GetCurrentLight()->GetComponent<LightComponent>();
    //Shader Uniform light matrix
    glBindBuffer(GL_UNIFORM_BUFFER, vubo);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(lightComponent->GetViewMatrix()));
    glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(lightComponent->GetProjectionMatrix()));
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
    //Shadow map framebuffer
    glViewport(0,0,4096,4096);
    glBindFramebuffer(GL_FRAMEBUFFER, shadowFrameBuffer);
    glEnable(GL_DEPTH_TEST);
    glClear(GL_DEPTH_BUFFER_BIT);
    OpenGLShader* openGLShader = static_cast<OpenGLShader*>(Context::GetContext()->resourceManager->FindShader("shadowmapShader"));
    for(auto& actor:Context::GetContext()->world->GetActors())
    {
        MeshComponent* meshComponent = actor->GetComponent<MeshComponent>();
        if (meshComponent == nullptr)
            continue;
        IMesh* mesh = meshComponent->GetMesh();
        openGLShader->UseProgram();
        openGLShader->SetMatrix4("mModel", meshComponent->GetMatrix());
        if(actor.get() == Context::GetContext()->world->GetSelectedActor())
        {
            OpenGLMaterial* material = static_cast<OpenGLMaterial*>(actor->GetComponent<MeshComponent>()->GetMaterial());
            MaterialParameter* parameter = material->GetParameter();
            GLint index = 0;
            if(parameter->dispTexture != nullptr)
            {
                openGLShader->SetBool("bDisp", true);
                openGLShader->SetFloat("heightScale", parameter->heightScale);
                openGLShader->SetInt("dispTexture", index);
                glActiveTexture(GL_TEXTURE0+index);
                parameter->dispTexture->Bind();
                index++;
            }
        }
        mesh->Bind();
        mesh->Draw();
        mesh->UnBind();
        openGLShader->EndProgam();
        glBindTexture(GL_TEXTURE_2D, 0);
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    //Shader Uniform camera matrix
    glBindBuffer(GL_UNIFORM_BUFFER, vubo);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(cameraComponent->GetViewMatrix()));
    glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(cameraComponent->GetProjectionMatrix()));
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
    //deferred
    if(bDeferred > 0)
    {
        //g-buffer frameRender
        glViewport(0,0,width,height);
        glBindFramebuffer(GL_FRAMEBUFFER, gfbo);
        glEnable(GL_DEPTH_TEST);
        Clear();
        glStencilFunc(GL_ALWAYS, 1, 0xFF);
        OpenGLShader* gbufferShader = static_cast<OpenGLShader*>(Context::GetContext()->resourceManager->FindShader("gbufferShader"));
        for(auto& actor :Context::GetContext()->world->GetActors())
        {
            MeshComponent* meshComponent = actor->GetComponent<MeshComponent>();
            if(meshComponent == nullptr)
                continue;
            IMesh* mesh = meshComponent->GetMesh();
            OpenGLMaterial* material = static_cast<OpenGLMaterial*>(meshComponent->GetMaterial());
            MaterialParameter* parameter = material->GetParameter();
            gbufferShader->UseProgram();
            if(actor.get() == Context::GetContext()->world->GetSelectedActor())
            {
                gbufferShader->SetVector3("diffuseColor", parameter->diffuseColor);
                gbufferShader->SetFloat("roughnessFactor", parameter->roughnessFactor);
                gbufferShader->SetFloat("metallicFactor", parameter->metallicFactor);
            }else
            {
                gbufferShader->SetVector3("diffuseColor",glm::vec3(1.0f));
                gbufferShader->SetFloat("roughnessFactor", 0.5f);
                gbufferShader->SetFloat("metallicFactor", 0.0f);
            }
            gbufferShader->SetMatrix4("mModel", meshComponent->GetMatrix());
            GLint index = 0;
            if(parameter->dispTexture != nullptr)
            {
                gbufferShader->SetBool("bDisp", true);
                gbufferShader->SetFloat("heightScale", parameter->heightScale);
                gbufferShader->SetInt("dispTexture", index);
                glActiveTexture(GL_TEXTURE0+index);
                parameter->dispTexture->Bind();
                index++;
            }
            if(parameter->diffuseTexture != nullptr)
            {
                gbufferShader->SetBool("bDiffuse", true);
                gbufferShader->SetInt("diffuseTexture", index);
                glActiveTexture(GL_TEXTURE0+index);
                parameter->diffuseTexture->Bind();
                index++;
            }
            if(parameter->normalTexture != nullptr)
            {
                gbufferShader->SetBool("bNormal", true);
                gbufferShader->SetInt("normalTexture", index);
                glActiveTexture(GL_TEXTURE0+index);
                parameter->normalTexture->Bind();
                index++;
            }
            if(parameter->roughnessTextrue != nullptr)
            {
                gbufferShader->SetBool("bRoughness", true);
                gbufferShader->SetInt("roughnessTexture", index);
                parameter->roughnessFactor = 1.0f;
                glActiveTexture(GL_TEXTURE0+index);
                parameter->roughnessTextrue->Bind();
                index++;
            }
            if(parameter->metallicTexture != nullptr)
            {
                gbufferShader->SetBool("bMetallic", true);
                gbufferShader->SetInt("metallicTexture", index);
                parameter->metallicFactor = 1.0f;
                glActiveTexture(GL_TEXTURE0+index);
                parameter->metallicTexture->Bind();
                index++;
            }
            if(parameter->aoTexture != nullptr)
            {
                gbufferShader->SetBool("bAo", true);
                gbufferShader->SetInt("aoTexture", index);
                glActiveTexture(GL_TEXTURE0+index);
                parameter->aoTexture->Bind();
                index++;
            }
            if(actor.get() == Context::GetContext()->world->GetSelectedActor())
            {
                glStencilMask(0xFF);
                mesh->Bind();
                mesh->Draw();
                mesh->UnBind();
            }
            else
            {
                glStencilMask(0x00);
                mesh->Bind();
                mesh->Draw();
                mesh->UnBind();
            }
            gbufferShader->SetBool("bDisp", false);
            gbufferShader->SetBool("bDiffuse", false);
            gbufferShader->SetBool("bNormal", false);
            gbufferShader->SetBool("bRoughness", false);
            gbufferShader->SetBool("bMetallic", false);            
            gbufferShader->SetBool("bAo", false);
            gbufferShader->EndProgam();
            glBindTexture(GL_TEXTURE_2D, 0);
        }
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        //ssao buffer
        glBindFramebuffer(GL_FRAMEBUFFER, ssaofbo);
        glDisable(GL_DEPTH_TEST);
        glClear(GL_COLOR_BUFFER_BIT);
        OpenGLShader* ssaoShader = static_cast<OpenGLShader*>(Context::GetContext()->resourceManager->FindShader("ssaoShader"));
        //uniform
        ssaoShader->UseProgram();
        ssaoShader->SetMatrix4("cameraView", cameraComponent->GetViewMatrix());
        ssaoShader->SetMatrix4("cameraProjection", cameraComponent->GetProjectionMatrix());
        for (unsigned int i = 0; i < 64; ++i)
            ssaoShader->SetVector3("samples[" + std::to_string(i) + "]", ssaoKernel[i]);
        glm::vec2 screenSize(width, height);
        ssaoShader->SetFloat("radius", ssaoRadius);
        ssaoShader->SetFloat("bias", ssaobias);
        ssaoShader->SetVector2("screenSize", screenSize);
        //texture
        GLint index = 0;
        ssaoShader->SetInt("gPosition", index);
        glActiveTexture(GL_TEXTURE0+index);
        glBindTexture(GL_TEXTURE_2D, gpos);
        index++;
        ssaoShader->SetInt("gNormal", index);
        glActiveTexture(GL_TEXTURE0+index);
        glBindTexture(GL_TEXTURE_2D, gnor);
        index++;
        ssaoShader->SetInt("noise", index);
        glActiveTexture(GL_TEXTURE0+index);
        glBindTexture(GL_TEXTURE_2D, noiseTexture);
        rendererMesh->Bind();
        rendererMesh->Draw();
        rendererMesh->UnBind();
        ssaoShader->EndProgam();
        glBindTexture(GL_TEXTURE_2D, 0);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        //ssao blur buffer
        glBindFramebuffer(GL_FRAMEBUFFER, ssaoblurfbo);
        glDisable(GL_DEPTH_TEST);
        glClear(GL_COLOR_BUFFER_BIT);
        OpenGLShader* ssaoblurShader = static_cast<OpenGLShader*>(Context::GetContext()->resourceManager->FindShader("ssaoblurShader"));
        
        ssaoblurShader->UseProgram();
        //texture
        index = 0;
        ssaoblurShader->SetInt("ssaoInput", index);
        glActiveTexture(GL_TEXTURE0+index);
        glBindTexture(GL_TEXTURE_2D, ssaocbo);
        index++;
        ssaoblurShader->SetInt("gAmbientOcclusion", index);
        glActiveTexture(GL_TEXTURE0+index);
        glBindTexture(GL_TEXTURE_2D, gORMI);
        rendererMesh->Bind();
        rendererMesh->Draw();
        rendererMesh->UnBind();
        ssaoblurShader->EndProgam();
        glBindTexture(GL_TEXTURE_2D, 0);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        //copy texture
        glCopyImageSubData(ssaoblurcbo, GL_TEXTURE_2D, 0, 0,0,0,
                        texViewOcclusion,GL_TEXTURE_2D, 0, 0,0,0,
                        width, height, 1);
        glCopyImageSubData(gORMI, GL_TEXTURE_2D, 0, 0,0,0,
                        texViewRoughness,GL_TEXTURE_2D, 0, 0,0,0,
                        width, height, 1);
        glCopyImageSubData(gORMI, GL_TEXTURE_2D, 0, 0,0,0,
                        texViewMetallic,GL_TEXTURE_2D, 0, 0,0,0,
                        width, height, 1);

        //deferred Render
        glBindFramebuffer(GL_FRAMEBUFFER, dfbo);
        glDisable(GL_DEPTH_TEST);
        glClear(GL_COLOR_BUFFER_BIT);
        MeshComponent* meshComponent = Context::GetContext()->world->GetSelectedActor()->GetComponent<MeshComponent>();
        MaterialParameter* parameter = static_cast<OpenGLMaterial*>(meshComponent->GetMaterial())->GetParameter();
        OpenGLShader* deferredShader = static_cast<OpenGLShader*>(Context::GetContext()->resourceManager->FindShader("deferredShader"));
        deferredShader->UseProgram();
        glm::mat4 lightView = lightComponent->GetViewMatrix();
        glm::mat4 lightProjection = lightComponent->GetProjectionMatrix();
        deferredShader->SetInt("shader", parameter->shader);
        deferredShader->SetMatrix4("mLightMatrix", lightProjection*lightView);
        deferredShader->SetVector3("directionalLight", lightComponent->GetDirection());
        deferredShader->SetVector3("cameraPosition", cameraComponent->GetTransform().position);
        //light
        deferredShader->SetFloat("lightIntensity",*lightComponent->GetIntensity());
        deferredShader->SetVector3("lightColor",glm::make_vec3(lightComponent->GetColor()));
        deferredShader->SetVector3("lightAmbient", glm::make_vec3(lightComponent->GetAmbient()));
        //material
        deferredShader->SetVector3("ambientColor", parameter->ambientColor);
        deferredShader->SetVector3("diffuseColor", parameter->diffuseColor);
        deferredShader->SetVector3("specularColor", parameter->specularColor);
        deferredShader->SetFloat("specularShininess", parameter->specularShininess);
        deferredShader->SetFloat("refractionFactor", parameter->refraction);
        //texture
        index = 0;
        deferredShader->SetInt("gPosition", index);
        glActiveTexture(GL_TEXTURE0+index);
        glBindTexture(GL_TEXTURE_2D, gpos);
        index++;
        deferredShader->SetInt("gNormal", index);
        glActiveTexture(GL_TEXTURE0+index);
        glBindTexture(GL_TEXTURE_2D, gnor);
        index++;
        deferredShader->SetInt("gDiffuse", index);
        glActiveTexture(GL_TEXTURE0+index);
        glBindTexture(GL_TEXTURE_2D, gdiff);
        index++;
        deferredShader->SetInt("gORMI", index);
        glActiveTexture(GL_TEXTURE0+index);
        glBindTexture(GL_TEXTURE_2D, gORMI);
        index++;
        deferredShader->SetInt("occlusionTexture", index);
        glActiveTexture(GL_TEXTURE0+index);
        glBindTexture(GL_TEXTURE_2D, ssaoblurcbo);
        index++;
        deferredShader->SetInt("shadowmapTexture", index);
        glActiveTexture(GL_TEXTURE0+index);
        glBindTexture(GL_TEXTURE_2D, shadowFrameTexture);
        index++;
        deferredShader->SetInt("illuminanceMap", index);
        glActiveTexture(GL_TEXTURE0+index);
        lightComponent->GetHDRITexture()->Bind();
        //mesh
        rendererMesh->Bind();
        rendererMesh->Draw();
        rendererMesh->UnBind();
        deferredShader->EndProgam();
        glBindTexture(GL_TEXTURE_2D, 0);
        //skybox forward
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LEQUAL);
        lightComponent->Draw();
        //stencilRender forward
        if (Context::GetContext()->world->GetSelectedActor() != nullptr && bStencil)
        {
            auto actor = Context::GetContext()->world->GetSelectedActor();
            MeshComponent* meshComponent = actor->GetComponent<MeshComponent>();
            OpenGLMaterial* material = static_cast<OpenGLMaterial*>(meshComponent->GetMaterial());
            MaterialParameter* parameter =  material->GetParameter();
            glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
            glStencilMask(0x00);
            glDisable(GL_DEPTH_TEST);
            parameter->bStencil = true;
            actor->Draw();
            parameter->bStencil = false;
            glStencilMask(0xFF);
            glStencilFunc(GL_ALWAYS, 0, 0xFF);
            glEnable(GL_DEPTH_TEST);
        }
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        finalColor = dcbo;
    }else
    {
        //forward frameRender
        glViewport(0,0,width,height);
        glBindFramebuffer(GL_FRAMEBUFFER, fbo);
        glEnable(GL_DEPTH_TEST);
        Clear();
        std::map<float, Actor*> alphaSorted;
        //colorRender
        for(auto& actor:Context::GetContext()->world->GetActors())
        {
            MeshComponent* meshComponent = actor->GetComponent<MeshComponent>();
            if (meshComponent == nullptr)
                continue;
            //alphaActor Check
            OpenGLMaterial* material = static_cast<OpenGLMaterial*>(meshComponent->GetMaterial());
            glm::vec3 cameraPos = Context::GetContext()->world->GetCurrentCamera()->GetComponent<CameraComponent>()->GetTransform().position;
            if(material->GetParameter()->type == MaterialType::Transparent)
            {
                float distance = glm::length(cameraPos - meshComponent->GetTransform().position);
                alphaSorted[distance] = actor.get();
                continue;
            }
            //DrawMesh
            if(actor.get() == Context::GetContext()->world->GetSelectedActor())
            {
                glStencilFunc(GL_ALWAYS, 1, 0xFF);
                glStencilMask(0xFF);
                actor->Draw();//sphere
            }
            else
            {
                glStencilMask(0x00);
                actor->Draw();//plane
            }
        }
        //alphaRender
        for(std::map<float, Actor*>::reverse_iterator it = alphaSorted.rbegin(); it !=alphaSorted.rend(); ++it)
        {
            glStencilFunc(GL_ALWAYS, 1, 0xFF);
            glStencilMask(0xFF);
            it->second->Draw();
        }
        //skybox
        LightComponent* light = Context::GetContext()->world->GetCurrentLight()->GetComponent<LightComponent>();
        glDepthFunc(GL_LEQUAL);
        glStencilMask(0x00);
        light->Draw();
        glDepthFunc(GL_LESS);
        //stencilRender
        if (Context::GetContext()->world->GetSelectedActor() != nullptr && bStencil)
        {
            auto actor = Context::GetContext()->world->GetSelectedActor();
            MeshComponent* meshComponent = actor->GetComponent<MeshComponent>();
            OpenGLMaterial* material = static_cast<OpenGLMaterial*>(meshComponent->GetMaterial());
            MaterialParameter* parameter =  material->GetParameter();
            glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
            glStencilMask(0x00);
            glDisable(GL_DEPTH_TEST);
            parameter->bStencil = true;
            actor->Draw();
            parameter->bStencil = false;
            glStencilMask(0xFF);
            glStencilFunc(GL_ALWAYS, 0, 0xFF);
            glEnable(GL_DEPTH_TEST);
        }
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        finalColor = cbo;
    }
    //posteffect
    glBindFramebuffer(GL_FRAMEBUFFER, pfbo);
    glDisable(GL_DEPTH_TEST);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    rendererShader->UseProgram();
    OpenGLShader* rendererOpenGLShader = static_cast<OpenGLShader*>(rendererShader.get());
    rendererOpenGLShader->SetInt("postEffecttype", postEffect);
    rendererOpenGLShader->SetInt("toneType", toneType);
    rendererOpenGLShader->SetFloat("exposure", exposure);
    rendererOpenGLShader->SetInt("screenTexture", 0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, finalColor);
    rendererMesh->Bind();
    rendererMesh->Draw();
    rendererMesh->UnBind();
    rendererShader->EndProgam();
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void OpenGLRenderer::Update()
{
    for(auto& actor:Context::GetContext()->world->GetActors())
    {
        actor->Update();
    }
}