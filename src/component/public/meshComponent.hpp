#pragma once
#include "sceneComponent.hpp"
#include "mesh.hpp"
#include "material.hpp"

class MeshComponent:public SceneComponent
{
    public:
        MeshComponent(IMesh* m, IMaterial* mat):mesh(m), material(mat){}
        void Draw() override;
    private:
        IMesh* mesh;
        IMaterial* material;
};