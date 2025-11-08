#pragma once
#include "sceneComponent.hpp"
#include "mesh.hpp"

class MeshComponent:public SceneComponent
{
    public:
        MeshComponent(IMesh* m = nullptr):mesh(m){}
        void Draw() override;
    private:
        IMesh* mesh;
};