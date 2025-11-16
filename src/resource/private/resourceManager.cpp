#include "resourceManager.hpp"

ResourceManager::ResourceManager()
{
    //create StandardMesh
    auto sphere = std::make_unique<ShpereMesh>(32,32,1.0f);
    auto cube = std::make_unique<CubeMesh>();
    auto coneMesh = std::make_unique<ConeMesh>(3,3.0f,1.0f);
    auto cylinderMesh = std::make_unique<CylinderMesh>(32, 2.0f, 2.0f);
    auto planeMesh = std::make_unique<PlaneMesh>(10.0f);
    auto skyboxMesh = std::make_unique<CubeMesh>();
    meshes.emplace("sphere", std::move(sphere));
    meshes.emplace("cube", std::move(cube));
    meshes.emplace("cone", std::move(coneMesh));
    meshes.emplace("cylinder", std::move(cylinderMesh));
    meshes.emplace("plane", std::move(planeMesh));
    meshes.emplace("skybox", std::move(skyboxMesh));
    //create mercury texture
    const std::string texPath = "/textures/mercury.jpg";
    auto mercuryTexture = std::make_unique<OpenGLTexture>(texPath);
    ITexture* ptrTexture = mercuryTexture.get();
    textures.emplace("mercury", std::move(mercuryTexture));
    //create grass texture
    const std::string grasstexPath = "/textures/grass.png";
    auto grassTexture = std::make_unique<OpenGLTexture>(grasstexPath);
    ITexture* ptrgrassTexture = grassTexture.get();
    textures.emplace("grass", std::move(grassTexture));
    //create window texture
    const std::string windowtexPath = "/textures/blending_transparent_window.png";
    auto windowTexture = std::make_unique<OpenGLTexture>(windowtexPath);
    ITexture* ptrwindowTexture = windowTexture.get();
    textures.emplace("blending_transparent_window", std::move(windowTexture));
    //create container texture
    const std::string containerPath = "/textures/container2.png";
    auto containerTexture = std::make_unique<OpenGLTexture>(containerPath);
    ITexture* ptrcontainerTexture = containerTexture.get();
    textures.emplace("container", std::move(containerTexture));
    //create container spec texture
    const std::string containerspecPath = "/textures/container2_specular.png";
    auto containerspecTexture = std::make_unique<OpenGLTexture>(containerspecPath);
    ITexture* ptrcontainerspecTexture = containerspecTexture.get();
    textures.emplace("containerspec", std::move(containerspecTexture));
    //create skybox texture
    std::vector<std::string> skyboxPaths
                                {
                                    "/textures/skybox/right.jpg",
                                    "/textures/skybox/left.jpg",
                                    "/textures/skybox/top.jpg",
                                    "/textures/skybox/bottom.jpg",
                                    "/textures/skybox/front.jpg",
                                    "/textures/skybox/back.jpg"
                                };
    auto skyboxTexture = std::make_unique<OpenGLCubeTexture>(skyboxPaths);
    textures.emplace("skybox", std::move(skyboxTexture));

    //create standard shader
    const std::string vsPath = "/shader/standard.vert";
    const std::string fsPath = "/shader/standard.frag";
    auto standardShader = std::make_unique<OpenGLShader>(vsPath, fsPath);
    IShader* ptrShader = standardShader.get();
    shaders.emplace("standardShader", std::move(standardShader));
    //create skybox shader
    const std::string skyboxvsPath = "/shader/skybox.vert";
    const std::string skyboxfsPath = "/shader/skybox.frag";
    auto skyboxhader = std::make_unique<OpenGLShader>(skyboxvsPath, skyboxfsPath);
    shaders.emplace("skyboxShader", std::move(skyboxhader));

    //create standard material
    auto standardMaterial = std::make_unique<OpenGLMaterial>(ptrShader);
    IMaterial* ptrMaterial = standardMaterial.get();
    materials.emplace("standardMaterial", std::move(standardMaterial));
    OpenGLMaterial* mat = static_cast<OpenGLMaterial*>(ptrMaterial);
    mat->AddTexture(ptrTexture, 1);
    //create planeMaterial
    auto planeMaterial = std::make_unique<OpenGLMaterial>(ptrShader);
    materials.emplace("planeMaterial", std::move(planeMaterial));
    //create grassMaterial
    auto grassMaterial = std::make_unique<OpenGLMaterial>(ptrShader);
    IMaterial* ptrgrassMaterial = grassMaterial.get();
    materials.emplace("grassMaterial", std::move(grassMaterial));
    mat = static_cast<OpenGLMaterial*>(ptrgrassMaterial);
    mat->AddTexture(ptrgrassTexture, 1);
    mat->GetParameter()->type = MaterialType::Transparent;
    //create windowMaterial
    auto windowMaterial = std::make_unique<OpenGLMaterial>(ptrShader);
    IMaterial* ptrwindowMaterial = windowMaterial.get();
    materials.emplace("windowMaterial", std::move(windowMaterial));
    mat = static_cast<OpenGLMaterial*>(ptrwindowMaterial);
    mat->AddTexture(ptrwindowTexture, 1);
    mat->GetParameter()->type = MaterialType::Transparent;
}

IMesh* ResourceManager::FindMesh(const std::string name)
{
    auto it = meshes.find(name);
    IMesh* result = nullptr;
    if(it != meshes.end())
    {
        result = it->second.get();
    }
    return result;
}

IShader* ResourceManager::FindShader(const std::string name)
{
    auto it = shaders.find(name);
    IShader* result = nullptr;
    if(it != shaders.end())
    {
        result = it->second.get();
    }
    return result;
}

ITexture* ResourceManager::FindTexture(const std::string name)
{
    auto it = textures.find(name);
    ITexture* result = nullptr;
    if(it != textures.end())
    {
        result = it->second.get();
    }
    return result;
}

IMaterial* ResourceManager::FindMaterial(const std::string name)
{
    auto it = materials.find(name);
    IMaterial* result = nullptr;
    if(it != materials.end())
    {
        result = it->second.get();
    }
    return result;
}