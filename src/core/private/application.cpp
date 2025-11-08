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
    cameraComponent->SetPosition(glm::vec3(0.0f, 5.0f, 15.0f));
    //create Light
    actor = world->CreateActor("Light Actor");
    auto lightComponent = actor->AddComponent<LightComponent>();
    //create Stard Actor;
    auto it = resourceManager->GetStandardMeshes().find("sphere");
    if(it != resourceManager->GetStandardMeshes().end())
    {
        auto actor = world->CreateActor("Sphere Actor");
        actor->AddComponent<MeshComponent>(it->second.get(), nullptr);
    }
    it = resourceManager->GetStandardMeshes().find("cube");
    if(it != resourceManager->GetStandardMeshes().end())
    {
        auto actor = world->CreateActor("Cube Actor");
        actor->AddComponent<MeshComponent>(it->second.get(), nullptr);
    }
    it = resourceManager->GetStandardMeshes().find("cone");
    if(it != resourceManager->GetStandardMeshes().end())
    {
        auto actor = world->CreateActor("Cone Actor");
        actor->AddComponent<MeshComponent>(it->second.get(), nullptr);
    }
    it = resourceManager->GetStandardMeshes().find("cylinder");
    if(it != resourceManager->GetStandardMeshes().end())
    {
        auto actor = world->CreateActor("Cylinder Actor");
        actor->AddComponent<MeshComponent>(it->second.get(), nullptr);
    }
    it = resourceManager->GetStandardMeshes().find("plane");
    if(it != resourceManager->GetStandardMeshes().end())
    {
        auto actor = world->CreateActor("plane Actor");
        actor->AddComponent<MeshComponent>(it->second.get(), nullptr);
    }
    Loop();
}

void Application::Loop()
{
    while(!window.ShouldClose())
    {
        renderer->Draw();
        world->Update();
        window.Update();
    }
    
}

int main()
{
    Application application;
    return 0;
}

