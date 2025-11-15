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
        actor->GetComponent<MeshComponent>()->SetPosition(glm::vec3(0.0f, 2.0f, 0.0f));
        actor->GetComponent<MeshComponent>()->SetScale(glm::vec3(2.0f));
        world->SetSelectedActor(actor);
    }
    //create plane actor
    it = resourceManager->GetMeshes().find("plane");
    if(it != resourceManager->GetMeshes().end())
    {
        auto actor = world->CreateActor("Plane Actor");
        actor->AddComponent<MeshComponent>(it->second.get(), planeMaterial);
    }
    /*
    //grass Test
    auto grassMaterial = resourceManager->FindMaterial("grassMaterial");
    for(size_t i = 0; i< 50; i++)
    {
        it = resourceManager->GetMeshes().find("plane");
        if(it != resourceManager->GetMeshes().end())
        {
            auto actor = world->CreateActor("Plane Actor");
            actor->AddComponent<MeshComponent>(it->second.get(), grassMaterial);
            actor->GetComponent<MeshComponent>()->SetScale(glm::vec3(0.25f));
            actor->GetComponent<MeshComponent>()->SetRotation(glm::vec3(90.0f,0.0f,180.0f));
            double random_x = static_cast<double>(rand()) / RAND_MAX;
            double x = (random_x * 10.0f) - 5.0f;
            double random_z = static_cast<double>(rand()) / RAND_MAX;
            double z = (random_z * 10.0f) - 5.0f;
            actor->GetComponent<MeshComponent>()->SetPosition(glm::vec3(x, 2.0f, z));
        }
    }
    //window Test
    auto windowMaterial = resourceManager->FindMaterial("windowMaterial");
    for(size_t i = 0; i< 10; i++)
    {
        it = resourceManager->GetMeshes().find("plane");
        if(it != resourceManager->GetMeshes().end())
        {
            auto actor = world->CreateActor("Plane Actor");
            actor->AddComponent<MeshComponent>(it->second.get(), windowMaterial);
            actor->GetComponent<MeshComponent>()->SetScale(glm::vec3(0.2f));
            actor->GetComponent<MeshComponent>()->SetRotation(glm::vec3(90.0f,0.0f,180.0f));
            
            double random_x = static_cast<double>(rand()) / RAND_MAX;
            double x = (random_x * 8.0f) - 4.0f;
            double random_z = static_cast<double>(rand()) / RAND_MAX;
            double z = (random_z * 8.0f) - 4.0f;
            actor->GetComponent<MeshComponent>()->SetPosition(glm::vec3(x, 2.5f, z));
        }
    }
    */
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

