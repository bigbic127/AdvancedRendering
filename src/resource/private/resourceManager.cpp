#include "resourceManager.hpp"

ResourceManager::ResourceManager()
{
    //create StandardMesh
    auto sphere = std::make_unique<ShpereMesh>(128,128,1.0f);
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
    //create ganges texture
    auto gangesDiffuseTexture = std::make_unique<OpenGLTexture>("/textures/ganges_river_pebbles_diff_2k.png");
    ITexture* ptrgangesDiffTexture = gangesDiffuseTexture.get();
    textures.emplace("ganges_river_pebbles_diff_2k", std::move(gangesDiffuseTexture));
    auto gangesroughTexture = std::make_unique<OpenGLTexture>("/textures/ganges_river_pebbles_rough_2k.png");
    ITexture* ptrgangesroughTexture = gangesroughTexture.get();
    textures.emplace("ganges_river_pebbles_rough_2k", std::move(gangesroughTexture));
    auto gangesnorTexture = std::make_unique<OpenGLTexture>("/textures/ganges_river_pebbles_nor_gl_2k.png");
    ITexture* ptrgangesnorTexture = gangesnorTexture.get();
    textures.emplace("ganges_river_pebbles_nor_gl_2k", std::move(gangesnorTexture));
    auto gangesaoTexture = std::make_unique<OpenGLTexture>("/textures/ganges_river_pebbles_ao_2k.png");
    ITexture* ptrgangesaoTexture = gangesaoTexture.get();
    textures.emplace("ganges_river_pebbles_ao_2k", std::move(gangesaoTexture));
    auto gangesdispTexture = std::make_unique<OpenGLTexture>("/textures/ganges_river_pebbles_disp_2k.png");
    ITexture* ptrgangesdispTexture = gangesdispTexture.get();
    textures.emplace("ganges_river_pebbles_disp_2k", std::move(gangesdispTexture));
    auto gangesarmTexture = std::make_unique<OpenGLTexture>("/textures/ganges_river_pebbles_arm_2k.png");
    ITexture* ptrgangesarmTexture = gangesarmTexture.get();
    textures.emplace("ganges_river_pebbles_arm_2k", std::move(gangesarmTexture));
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
    auto skyboxshader = std::make_unique<OpenGLShader>(skyboxvsPath, skyboxfsPath);
    shaders.emplace("skyboxShader", std::move(skyboxshader));
    //create shadowmap shader
    const std::string shadowmapvsPath = "/shader/shadowmap.vert";
    const std::string shadowmapfsPath = "/shader/shadowmap.frag";
    auto shadowmapshader = std::make_unique<OpenGLShader>(shadowmapvsPath, shadowmapfsPath);
    shaders.emplace("shadowmapShader", std::move(shadowmapshader));
    //create depthmap shader
    const std::string depthmapvsPath = "/shader/depthmap.vert";
    const std::string depthmapfsPath = "/shader/depthmap.frag";
    auto depthmapshader = std::make_unique<OpenGLShader>(depthmapvsPath, depthmapfsPath);
    shaders.emplace("depthmapShader", std::move(depthmapshader));
    //create gbuffer shader
    const std::string gbuffervsPath = "/Shader/gbuffer.vert";
    const std::string gbufferfsPath = "/Shader/gbuffer.frag";
    auto gbuffershader = std::make_unique<OpenGLShader>(gbuffervsPath, gbufferfsPath);
    shaders.emplace("gbufferShader", std::move(gbuffershader));
    //create deferred shader
    const std::string deferredvsPath = "/Shader/deferred.vert";
    const std::string deferredfsPath = "/Shader/deferred.frag";
    auto deferredshader = std::make_unique<OpenGLShader>(deferredvsPath, deferredfsPath);
    shaders.emplace("deferredShader", std::move(deferredshader));
    
    //create standard material
    auto standardMaterial = std::make_unique<OpenGLMaterial>(ptrShader);
    IMaterial* ptrMaterial = standardMaterial.get();
    materials.emplace("standardMaterial", std::move(standardMaterial));
    OpenGLMaterial* mat = static_cast<OpenGLMaterial*>(ptrMaterial);
    mat->AddTexture(ptrgangesDiffTexture, 1);
    mat->AddTexture(ptrgangesroughTexture, 2);
    mat->AddTexture(ptrgangesarmTexture, 3);
    mat->AddTexture(ptrgangesnorTexture, 4);
    mat->AddTexture(ptrgangesaoTexture, 5);
    mat->AddTexture(ptrgangesdispTexture, 6);
    //create planeMaterial
    auto planeMaterial = std::make_unique<OpenGLMaterial>(ptrShader);
    materials.emplace("planeMaterial", std::move(planeMaterial));
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