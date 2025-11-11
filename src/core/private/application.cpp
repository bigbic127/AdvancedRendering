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
    cameraComponent->SetPosition(glm::vec3(0.0f, 5.0f, 10.0f));
    world->SetCurrentCamera(actor);
    //create Light
    actor = world->CreateActor("Light Actor");
    auto lightComponent = actor->AddComponent<DirectionalLightComponent>();
    world->SetCurrentLight(actor);
    //get standardMaterial
    auto itMat = resourceManager->GetMaterials().find("standardMaterial");
    IMaterial* standardMaterial = nullptr;
    if(itMat != resourceManager->GetMaterials().end())
    {
        standardMaterial = itMat->second.get();
    }
    itMat = resourceManager->GetMaterials().find("planeMaterial");
    IMaterial* planeMaterial = nullptr;
    if(itMat != resourceManager->GetMaterials().end())
    {
        planeMaterial = itMat->second.get();
    }
    //create StardardActor;
    auto it = resourceManager->GetMeshes().find("sphere");
    if(it != resourceManager->GetMeshes().end())
    {
        auto actor = world->CreateActor("Mesh Actor");
        actor->AddComponent<MeshComponent>(it->second.get(), standardMaterial);
        actor->GetComponent<MeshComponent>()->SetPosition(glm::vec3(0.0f, 2.0f, 0.0f));
        actor->GetComponent<MeshComponent>()->SetScale(glm::vec3(2.0f));
        world->SetSelectedActor(actor);
    }
    it = resourceManager->GetMeshes().find("plane");
    if(it != resourceManager->GetMeshes().end())
    {
        auto actor = world->CreateActor("Plane Actor");
        actor->AddComponent<MeshComponent>(it->second.get(), planeMaterial);
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

