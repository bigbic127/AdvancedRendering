#include "meshComponent.hpp"

void MeshComponent::Draw()
{
    if(mesh != nullptr)
    {
        //openGL
        if(material != nullptr)
        {
            OpenGLMaterial* openGLMaterial = static_cast<OpenGLMaterial*>(material);
            openGLMaterial->Bind();
            openGLMaterial->Draw(this);
            mesh->Bind();
            mesh->Draw();
            mesh->UnBind();
            openGLMaterial->UnBind();
        }
    }
}