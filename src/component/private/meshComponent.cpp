#include "meshComponent.hpp"
#include "context.hpp"
#include "world.hpp"
#include "actor.hpp"
#include "cameraComponent.hpp"

void MeshComponent::Draw()
{
    if(mesh != nullptr)
    {
        OpenGLMaterial* mat = dynamic_cast<OpenGLMaterial*>(material);
        OpenGLShader* shader = mat->GetShader();
        CameraComponent* camComponent = Context::GetContext()->world->GetCurrentCamera()->GetComponent<CameraComponent>();
        shader->UseProgram();
        shader->SetMatrix4("mModel", GetMatrix());
        shader->SetMatrix4("mView", camComponent->GetViewMatrix());
        shader->SetMatrix4("mProjection", camComponent->GetProjectionMatrix());
        mesh->Bind();
        mesh->Draw();
        mesh->UnBind();
        shader->EndProgam();
    }
}