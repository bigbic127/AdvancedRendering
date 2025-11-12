#include "material.hpp"
#include "context.hpp"
#include "world.hpp"
#include "cameraComponent.hpp"
#include "meshComponent.hpp"
#include "lightComponent.hpp"
#include "texture.hpp"

void OpenGLMaterial::Bind()
{
    shader->UseProgram();
}

void OpenGLMaterial::Draw(MeshComponent* component)
{
    CameraComponent* camComponent = Context::GetContext()->world->GetCurrentCamera()->GetComponent<CameraComponent>();
    LightComponent* lightComponent = Context::GetContext()->world->GetCurrentLight()->GetComponent<LightComponent>();
    shader->SetMatrix4("mModel", component->GetMatrix());
    shader->SetMatrix4("mView", camComponent->GetViewMatrix());
    shader->SetMatrix4("mProjection", camComponent->GetProjectionMatrix());
    shader->SetVector3("cameraPosition",camComponent->GetTransform().position);
    shader->SetVector3("diffuseColor", parameter.diffuseColor);
    shader->SetFloat("lightIntensity",*lightComponent->GetIntensity());
    shader->SetVector3("lightColor",glm::make_vec3(lightComponent->GetColor()));
    shader->SetVector3("directionalLight",lightComponent->GetDirection());
    if (parameter.diffuseTexture != nullptr)
    {
        shader->SetBool("bDiffuse", true);
        parameter.diffuseTexture->Bind();
    }
    shader->SetBool("bStencil",parameter.bStencil);
    shader->SetVector3("stencilColor",parameter.stencilColor);
    shader->SetFloat("stencilOutline",parameter.stencilOutline);
}

void OpenGLMaterial::UnBind()
{
    if (parameter.diffuseTexture != nullptr)
    {
        shader->SetBool("bDiffuse", false);
        parameter.diffuseTexture->UnBind();
    }
    shader->EndProgam();
}

void OpenGLMaterial::AddTexture(ITexture* texture, int type)
{
    switch(type)
    {
        case 1:
        {
            GLint location = shader->GetLocation("diffuseTexture");
            glUniform1i(location, 0);
            glActiveTexture(GL_TEXTURE0);
            parameter.diffuseTexture = texture;
            break;
        }
        case 2:
        {
            GLint location = shader->GetLocation("roughnessTexture");
            glUniform1i(location, 1);
            glActiveTexture(GL_TEXTURE1);
            parameter.roughnessTextrue = texture;
            break;
        }
        case 3:
        {
            GLint location = shader->GetLocation("metallicTexture");
            glUniform1i(location, 2);
            glActiveTexture(GL_TEXTURE2);
            parameter.metallicTexture = texture;
            break;
        }
        case 4:
        {
            GLint location = shader->GetLocation("normalTexture");
            glUniform1i(location, 3);
            glActiveTexture(GL_TEXTURE3);
            parameter.normalTexture = texture;
            break;
        }
        default:break;
    }
}