#include "application.hpp"
#include "context.hpp"
#include "meshComponent.hpp"
#include "cameraComponent.hpp"
#include "lightComponent.hpp"
#include "actor.hpp"

Application::Application()
{
    if(window.Init())
    {
        renderer = std::make_unique<OpenGLRenderer>();
        resourceManager = std::make_unique<ResourceManager>();
        world = std::make_unique<World>();        
        context = std::make_unique<Context>(&window, renderer.get(), window.GetEditor(), world.get(), resourceManager.get());
        Init();
    }
}

Application::~Application()
{

}

void Application::Init()
{
    //create Camera
    auto actor = world->CreateActor("Camera Actor");
    auto cameraComponent = actor->AddComponent<CameraComponent>();
    cameraComponent->SetPosition(glm::vec3(0.0f, 5.0f, 20.0f));
    world->SetCurrentCamera(actor);
    //create Light
    actor = world->CreateActor("Light Actor");
    auto lightComponent = actor->AddComponent<LightComponent>();
    //get standardMaterial
    auto itMat = resourceManager->GetMaterials().find("standardMaterial");
    IMaterial* standardMaterial = nullptr;
    if(itMat != resourceManager->GetMaterials().end())
    {
        standardMaterial = itMat->second.get();
    }
    //create StardardActor;
    auto it = resourceManager->GetMeshes().find("sphere");
    if(it != resourceManager->GetMeshes().end())
    {
        auto actor = world->CreateActor("Sphere Actor");
        actor->AddComponent<MeshComponent>(it->second.get(), standardMaterial);
    }
    it = resourceManager->GetMeshes().find("cube");
    if(it != resourceManager->GetMeshes().end())
    {
        auto actor = world->CreateActor("Cube Actor");
        actor->AddComponent<MeshComponent>(it->second.get(), standardMaterial);
    }
    it = resourceManager->GetMeshes().find("cone");
    if(it != resourceManager->GetMeshes().end())
    {
        auto actor = world->CreateActor("Cone Actor");
        actor->AddComponent<MeshComponent>(it->second.get(), standardMaterial);
    }
    it = resourceManager->GetMeshes().find("cylinder");
    if(it != resourceManager->GetMeshes().end())
    {
        auto actor = world->CreateActor("Cylinder Actor");
        actor->AddComponent<MeshComponent>(it->second.get(), standardMaterial);
    }
    it = resourceManager->GetMeshes().find("plane");
    if(it != resourceManager->GetMeshes().end())
    {
        auto actor = world->CreateActor("plane Actor");
        actor->AddComponent<MeshComponent>(it->second.get(), standardMaterial);
    }
    Loop();
}

void Application::Loop()
{
    while(!window.ShouldClose())
    {
        world->Update();
        renderer->Update();
        renderer->Draw();
        window.Update();
    }
    
}

int main()
{
    Application application;
    return 0;
}

