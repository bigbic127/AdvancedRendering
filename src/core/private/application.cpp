#include "application.hpp"
#include "context.hpp"
#include "meshComponent.hpp"
#include "cameraComponent.hpp"
#include "lightComponent.hpp"
#include "actor.hpp"

#include <random>
#include <iomanip>

Application::Application()
{
    if(window.Init())
    {
        resourceManager = std::make_unique<ResourceManager>();
        renderer = std::make_unique<OpenGLRenderer>();
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
    lightComponent->SetPosition(glm::vec3(0.0f, 5.0f, 0.0f));
    lightComponent->SetRotation(glm::vec3(35.0f, -40.0f, 0.0f));
    lightComponent->SetIntensity(10.0f);
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
    //create sphere actor;
    auto it = resourceManager->GetMeshes().find("sphere");
    if(it != resourceManager->GetMeshes().end())
    {
        auto actor = world->CreateActor("Mesh Actor");
        actor->AddComponent<MeshComponent>(it->second.get(), standardMaterial);
        actor->GetComponent<MeshComponent>()->SetPosition(glm::vec3(0.0f, 2.5f, 0.0f));
        actor->GetComponent<MeshComponent>()->SetScale(glm::vec3(2.5f));
        world->SetSelectedActor(actor);
    }
    //create plane actor
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
    renderer->Begin();
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

