#include "lightComponent.hpp"
#include "mesh.hpp"
#include <string>
#include "shader.hpp"
#include "context.hpp"
#include "resourceManager.hpp"
#include "logger.hpp"

LightComponent::LightComponent()
{
    skyboxMesh = Context::GetContext()->resourceManager->FindMesh("skybox");
    skyboxShader = Context::GetContext()->resourceManager->FindShader("skyboxShader");
    //static_cast<OpenGLShader*>(skyboxShader)->SetInt("skybox", 0);
    //glActiveTexture(GL_TEXTURE0);
    skyboxTexture = Context::GetContext()->resourceManager->FindTexture("skybox");
}

void LightComponent::Draw()
{
    if(skyboxTexture == nullptr)
        return;
    glCullFace(GL_FRONT);
    skyboxShader->UseProgram();
    skyboxMesh->Bind();
    skyboxTexture->Bind();
    OpenGLShader* shader = static_cast<OpenGLShader*>(skyboxShader);
    glm::mat4 mat(1.0f);
    mat = glm::scale(mat, glm::vec3(100.0f));
    shader->SetMatrix4("mModel", mat);
    skyboxMesh->Draw();
    skyboxTexture->UnBind();
    skyboxMesh->UnBind();
    skyboxShader->EndProgam();
    glCullFace(GL_BACK);
}


void LightComponent::Update()
{
    view = glm::lookAt(transform.position, targetPosition, glm::vec3(0.0f, 1.0f, 0.0f));
    float scale = 12.0f;
    projection = glm::ortho(-scale, scale, -scale, scale, 0.01f, 20.0f);
}
