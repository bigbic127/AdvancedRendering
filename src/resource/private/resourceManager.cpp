#include "resourceManager.hpp"

ResourceManager::ResourceManager()
{
    //create StandardMesh
    auto sphere = std::make_unique<ShpereMesh>(32,32,1.0f);
    auto cube = std::make_unique<CubeMesh>();
    auto coneMesh = std::make_unique<ConeMesh>(3,3.0f,1.0f);
    auto cylinderMesh = std::make_unique<CylinderMesh>(32, 2.0f, 2.0f);
    auto planeMesh = std::make_unique<PlaneMesh>(10.0f);
    meshes.emplace("sphere", std::move(sphere));
    meshes.emplace("cube", std::move(cube));
    meshes.emplace("cone", std::move(coneMesh));
    meshes.emplace("cylinder", std::move(cylinderMesh));
    meshes.emplace("plane", std::move(planeMesh));
    //create shader
    const std::string vsPath = "/shader/standard.vert";
    const std::string fsPath = "/shader/standard.frag";
    auto standardShader = std::make_unique<OpenGLShader>(vsPath, fsPath);
    shaders.emplace("standardShader", std::move(standardShader));
    //createMaterial
    
}
