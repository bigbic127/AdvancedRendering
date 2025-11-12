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
    //create texture
    const std::string texPath = "/textures/mercury.jpg";
    auto mercuryTexture = std::make_unique<OpenGLTexture>(texPath);
    ITexture* ptrTexture = mercuryTexture.get();
    textures.emplace("mercury", std::move(mercuryTexture));
    //create shader
    const std::string vsPath = "/shader/standard.vert";
    const std::string fsPath = "/shader/standard.frag";
    auto standardShader = std::make_unique<OpenGLShader>(vsPath, fsPath);
    IShader* ptrShader = standardShader.get();
    shaders.emplace("standardShader", std::move(standardShader));
    //create material
    auto standardMaterial = std::make_unique<OpenGLMaterial>(ptrShader);
    IMaterial* ptrMaterial = standardMaterial.get();
    materials.emplace("standardMaterial", std::move(standardMaterial));
    OpenGLMaterial* mat = static_cast<OpenGLMaterial*>(ptrMaterial);
    mat->AddTexture(ptrTexture, 1);
    //create planeMaterial
    auto planeMaterial = std::make_unique<OpenGLMaterial>(ptrShader);
    materials.emplace("planeMaterial", std::move(planeMaterial));
}
