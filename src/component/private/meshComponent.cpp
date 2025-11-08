#include "meshComponent.hpp"

void MeshComponent::Draw()
{
    if(mesh != nullptr)
    {
        mesh->Bind();
        mesh->Draw();
        mesh->UnBind();
    }
}