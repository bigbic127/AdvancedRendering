#include "renderer.hpp"
#include "glad/glad.h"
#include "context.hpp"
#include "world.hpp"
#include "resourceManager.hpp"
#include "material.hpp"
#include "meshComponent.hpp"
#include "cameraComponent.hpp"

OpenGLRenderer::OpenGLRenderer()
{
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
    glViewport(0, 0, width, height);
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
    
}

void OpenGLRenderer::Draw()
{
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
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
            if(material->GetParameter()->type == 1)
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
}

void OpenGLRenderer::Update()
{
    for(auto& actor:Context::GetContext()->world->GetActors())
    {
        actor->Update();
    }
}