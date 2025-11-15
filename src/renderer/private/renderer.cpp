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

OpenGLRenderer::OpenGLRenderer()
{
    //create mesh and shader
    rendererMesh = std::make_unique<QaudMesh>();
    const std::string vsPath = "/shader/posteffect.vert";
    const std::string fsPath = "/shader/posteffect.frag";
    rendererShader = std::make_unique<OpenGLShader>(vsPath, fsPath);
    static_cast<OpenGLShader*>(rendererShader.get())->SetInt("screenTexture", 0);
    glActiveTexture(GL_TEXTURE0);
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
}

OpenGLRenderer::~OpenGLRenderer()
{
    glDeleteFramebuffers(1, &fbo);
    glDeleteRenderbuffers(1, &rbo);
    glDeleteTextures(1, &cbo);
}

void OpenGLRenderer::CreateBuffer(int width, int height)
{
    this->width = width;
    this->height = height;
    //G-buffer
    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    //colorTexture
    glGenTextures(1, &cbo);
    glBindTexture(GL_TEXTURE_2D, cbo);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, 4, GL_RGBA8, width, height, GL_TRUE);
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
    glViewport(0, 0, width, height);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    //post framebuffer
    glGenFramebuffers(1, &fbo2);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo2);
    //colorTexture
    glGenTextures(1, &cbo2);
    glBindTexture(GL_TEXTURE_2D, cbo2);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, 4, GL_RGBA8, width, height, GL_TRUE);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, cbo2, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void OpenGLRenderer::ResizeBuffer(int width, int height)
{
    if(width == this->width && height == this->height) return;
    glDeleteFramebuffers(1, &fbo);
    glDeleteRenderbuffers(1, &rbo);
    glDeleteTextures(1, &cbo);
    CreateBuffer(width, height);
}

void OpenGLRenderer::Clear()
{
    glClearColor(0.7f,0.7f,0.7f,1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
}

void OpenGLRenderer::Begin()
{
    //shader uniform
    IShader* shader = Context::GetContext()->resourceManager->FindShader("standardShader");
    unsigned int shaderID = static_cast<OpenGLShader*>(shader)->GetID();
    unsigned int index = glGetUniformBlockIndex(shaderID, "mCamera");
    glUniformBlockBinding(shaderID, index, 0);
    //Shader Uniform buffers
    glGenBuffers(1, &ubo);
    glBindBuffer(GL_UNIFORM_BUFFER, ubo);
    glBufferData(GL_UNIFORM_BUFFER, 2*sizeof(glm::mat4), NULL, GL_STATIC_DRAW);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
    glBindBufferRange(GL_UNIFORM_BUFFER, 0, ubo, 0, 2*sizeof(glm::mat4));
}

void OpenGLRenderer::Draw()
{
    //Shader Uniform
    CameraComponent* cameraComponent = Context::GetContext()->world->GetCurrentCamera()->GetComponent<CameraComponent>();
    LightComponent* lightComponent = Context::GetContext()->world->GetCurrentLight()->GetComponent<LightComponent>();
    glBindBuffer(GL_UNIFORM_BUFFER, ubo);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(cameraComponent->GetViewMatrix()));
    glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(cameraComponent->GetProjectionMatrix()));
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glEnable(GL_DEPTH_TEST);
    Clear();
    std::map<float, Actor*> alphaSorted;
    //colorRender
    for(auto& actor:Context::GetContext()->world->GetActors())
    {
        MeshComponent* meshComponent = actor->GetComponent<MeshComponent>();
        if (meshComponent != nullptr)
        {
            //alphaActor Check
            OpenGLMaterial* material = static_cast<OpenGLMaterial*>(meshComponent->GetMaterial());
            glm::vec3 cameraPos = Context::GetContext()->world->GetCurrentCamera()->GetComponent<CameraComponent>()->GetTransform().position;
            if(material->GetParameter()->type == MaterialType::Transparent)
            {
                float distance = glm::length(cameraPos - meshComponent->GetTransform().position);
                alphaSorted[distance] = actor.get();
                continue;
            }
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
    //posteffect
    glBindFramebuffer(GL_FRAMEBUFFER, fbo2);
    glDisable(GL_DEPTH_TEST);
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    rendererShader->UseProgram();
    rendererMesh->Bind();
    static_cast<OpenGLShader*>(rendererShader.get())->SetInt("postEffecttype", postEffect);
    glBindTexture(GL_TEXTURE_2D, cbo);
    rendererMesh->Draw();
    rendererMesh->UnBind();
    rendererShader->EndProgam();
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    //skybox
    LightComponent* light = Context::GetContext()->world->GetCurrentLight()->GetComponent<LightComponent>();
    glDepthFunc(GL_LEQUAL);
    glDepthFunc(GL_LESS);
}

void OpenGLRenderer::Update()
{
    for(auto& actor:Context::GetContext()->world->GetActors())
    {
        actor->Update();
    }
}