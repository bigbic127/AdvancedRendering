#include "material.hpp"
#include "context.hpp"
#include "world.hpp"
#include "cameraComponent.hpp"
#include "meshComponent.hpp"

void OpenGLMaterial::Bind()
{
    shader->UseProgram();
}

void OpenGLMaterial::Draw(MeshComponent* component)
{
    CameraComponent* camComponent = Context::GetContext()->world->GetCurrentCamera()->GetComponent<CameraComponent>();
    shader->SetMatrix4("mModel", component->GetMatrix());
    shader->SetMatrix4("mView", camComponent->GetViewMatrix());
    shader->SetMatrix4("mProjection", camComponent->GetProjectionMatrix());
    shader->SetVector3("diffuseColor", parameter.diffuseColor);
    shader->SetVector3("cameraPosition",camComponent->GetTransform().position);
    //texture
}

void OpenGLMaterial::UnBind()
{
    shader->EndProgam();
}