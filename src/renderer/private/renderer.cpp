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
}

OpenGLRenderer::~OpenGLRenderer()
{
    glDeleteFramebuffers(1, &fbo);
    glDeleteRenderbuffers(1, &rbo);
    glDeleteTextures(1, &cbo);
    glDeleteFramebuffers(1, &fbo2);
    glDeleteTextures(1, &cbo2);
    glDeleteTextures(1, &shadowFrameBuffer);
    glDeleteTextures(1, &shadowFrameTexture);
    glDeleteFramebuffers(1, &gfbo);
    glDeleteTextures(1, &gpos);
    glDeleteTextures(1, &gnor);
    glDeleteTextures(1, &gdiff);
    glDeleteRenderbuffers(1, &grbo);
    glDeleteBuffers(1, &vubo);
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
    CreateGBuffer(width, height);
    glViewport(0, 0, width, height);
}

void OpenGLRenderer::CreateGBuffer(int width, int height)
{
    glGenFramebuffers(1, &gfbo);
    glBindFramebuffer(GL_FRAMEBUFFER, gfbo);
    //position
    glGenTextures(1, &gpos);
    glBindTexture(GL_TEXTURE_2D, gpos);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gpos, 0);
    //normal
    glGenTextures(1, &gnor);
    glBindTexture(GL_TEXTURE_2D, gnor);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, gnor, 0);
    //diffuse and specular
    glGenTextures(1, &gdiff);
    glBindTexture(GL_TEXTURE_2D, gdiff);
    glTexImage2D(GL_TEXTURE_2D, 0 , GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, gdiff, 0);
    //ambient occlusion
    glGenTextures(1, &gao);
    glBindTexture(GL_TEXTURE_2D, gao);
    glTexImage2D(GL_TEXTURE_2D, 0 , GL_R8, width, height, 0, GL_RED, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, GL_TEXTURE_2D, gao, 0);
    //
    unsigned int attachments[4] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3};
    glDrawBuffers(4, attachments);
    //depth
    glGenRenderbuffers(1, &grbo);
    glBindRenderbuffer(GL_RENDERBUFFER, grbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, grbo);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void OpenGLRenderer::ResizeBuffer(int width, int height)
{
    if(width == this->width && height == this->height) return;
    glDeleteFramebuffers(1, &fbo);
    glDeleteRenderbuffers(1, &rbo);
    glDeleteTextures(1, &cbo);
    glDeleteFramebuffers(1, &fbo2);
    glDeleteTextures(1, &cbo2);
    glDeleteFramebuffers(1, &gfbo);
    glDeleteTextures(1, &gpos);
    glDeleteTextures(1, &gnor);
    glDeleteTextures(1, &gdiff);
    glDeleteRenderbuffers(1, &grbo);
    CreateBuffer(width, height);
}

void OpenGLRenderer::Clear()
{
    glClearColor(0.7f,0.7f,0.7f,1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
}

void OpenGLRenderer::Begin()
{
    //shadowMap framebuffer
    glGenFramebuffers(1, &shadowFrameBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, shadowFrameBuffer);
    //shadowMap texture
    unsigned int shadow_width = 2048, shadow_height = 2048;
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
    glViewport(0,0,2048,2048);
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
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    //Shader Uniform camera matrix
    glBindBuffer(GL_UNIFORM_BUFFER, vubo);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(cameraComponent->GetViewMatrix()));
    glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(cameraComponent->GetProjectionMatrix()));
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    //g-buffer frameRender
    glViewport(0,0,width,height);
    glBindFramebuffer(GL_FRAMEBUFFER, gfbo);
    glEnable(GL_DEPTH_TEST);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
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
        if(parameter->aoTexture != nullptr)
        {
            gbufferShader->SetBool("bAo", true);
            gbufferShader->SetInt("aoTexture", index);
            glActiveTexture(GL_TEXTURE0+index);
            parameter->aoTexture->Bind();
            index++;
        }
        mesh->Bind();
        mesh->Draw();
        mesh->UnBind();
        gbufferShader->SetBool("bDisp", false);
        gbufferShader->SetBool("bDiffuse", false);
        gbufferShader->SetBool("bNormal", false);
        gbufferShader->SetBool("bAo", false);
        glBindTexture(GL_TEXTURE_2D, 0);
        gbufferShader->EndProgam();
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

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

    //posteffect
    glBindFramebuffer(GL_FRAMEBUFFER, fbo2);
    glDisable(GL_DEPTH_TEST);
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    rendererShader->UseProgram();
    OpenGLShader* rendererOpenGLShader = static_cast<OpenGLShader*>(rendererShader.get());
    rendererOpenGLShader->SetInt("postEffecttype", postEffect);
    rendererOpenGLShader->SetFloat("exposure", exposure);
    rendererOpenGLShader->SetInt("screenTexture", 0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, cbo);
    rendererMesh->Bind();
    rendererMesh->Draw();
    rendererMesh->UnBind();
    rendererShader->EndProgam();
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    //deferred
    if(bDeferred > 0)
    {
        glBindFramebuffer(GL_FRAMEBUFFER, fbo2);
        glDisable(GL_DEPTH_TEST);
        glClearColor(0.7f, 0.7f, 0.7f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        OpenGLShader* deferredShader = static_cast<OpenGLShader*>(Context::GetContext()->resourceManager->FindShader("deferredShader"));
        deferredShader->UseProgram();
        glm::mat4 lightView = lightComponent->GetViewMatrix();
        glm::mat4 lightProjection = lightComponent->GetProjectionMatrix();
        deferredShader->SetMatrix4("mLightMatrix", lightProjection*lightView);
        deferredShader->SetVector3("directionalLight", lightComponent->GetTransform().position);
        deferredShader->SetVector3("cameraPosition", cameraComponent->GetTransform().position);
        GLint index = 0;
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
        deferredShader->SetInt("gAmbientOcclusion", index);
        glActiveTexture(GL_TEXTURE0+index);
        glBindTexture(GL_TEXTURE_2D, gao);
        index++;
        deferredShader->SetInt("shadowmapTexture", index);
        glActiveTexture(GL_TEXTURE0+index);
        glBindTexture(GL_TEXTURE_2D, shadowFrameTexture);
        rendererMesh->Bind();
        rendererMesh->Draw();
        rendererMesh->UnBind();
        deferredShader->EndProgam();
        glBindTexture(GL_TEXTURE_2D, 0);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
}

void OpenGLRenderer::Update()
{
    for(auto& actor:Context::GetContext()->world->GetActors())
    {
        actor->Update();
    }
}