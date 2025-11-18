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
    //stencil
    shader->SetBool("bStencil",parameter.bStencil);
    shader->SetVector3("stencilColor",parameter.stencilColor);
    shader->SetFloat("stencilOutline",parameter.stencilOutline);
    if(parameter.bStencil)
        return;
    //shader->SetMatrix4("mView", camComponent->GetViewMatrix());
    //shader->SetMatrix4("mProjection", camComponent->GetProjectionMatrix());
    shader->SetVector3("cameraPosition",camComponent->GetTransform().position);
    //parameter
    shader->SetInt("shader", parameter.shader);
    shader->SetInt("type", parameter.type);
    shader->SetVector3("ambientColor", parameter.ambientColor);
    shader->SetVector4("diffuseColor", parameter.diffuseColor);
    shader->SetVector3("specularColor", parameter.specularColor);
    shader->SetFloat("specularShininess", parameter.specularShininess);

    shader->SetFloat("shininessFactor", parameter.shininess);
    shader->SetFloat("roughnessFactor", parameter.roughnessFactor);
    shader->SetFloat("metallicFactor", parameter.metallicFactor);
    //light
    shader->SetFloat("lightIntensity",*lightComponent->GetIntensity());
    shader->SetVector3("lightColor",glm::make_vec3(lightComponent->GetColor()));
    shader->SetVector3("lightAmbient", glm::make_vec3(lightComponent->GetAmbient()));
    
    int shaderIndex = 0;
    if(lightComponent->GetSkyboxTexture() != nullptr)
    {
        shader->SetBool("bSkybox", true);
        shader->SetFloat("refractionFactor", parameter.refraction);
        GLint location = shader->GetLocation("skyboxTexture");
        glUniform1i(location, shaderIndex);
        glActiveTexture(GL_TEXTURE0+shaderIndex);
        shaderIndex += 1;
        lightComponent->GetSkyboxTexture()->Bind();
    }
    shader->SetVector3("directionalLight",lightComponent->GetDirection());
    if (parameter.diffuseTexture != nullptr)
    {
        shader->SetBool("bDiffuse", true);
        GLint location = shader->GetLocation("diffuseTexture");
        glUniform1i(location, shaderIndex);
        glActiveTexture(GL_TEXTURE0+shaderIndex);
        shaderIndex += 1;
        parameter.diffuseTexture->Bind();
    }
}

void OpenGLMaterial::UnBind()
{
    LightComponent* lightComponent = Context::GetContext()->world->GetCurrentLight()->GetComponent<LightComponent>();
    if (parameter.diffuseTexture != nullptr)
    {
        shader->SetBool("bDiffuse", false);
        parameter.diffuseTexture->UnBind();
    }
    //skybox
    if(lightComponent->GetSkyboxTexture() != nullptr)
    {
        shader->SetBool("bSkybox", false);
        lightComponent->GetSkyboxTexture()->UnBind();
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