#include "world.hpp"
#include "glfw/glfw3.h"

Actor* World::CreateActor(const std::string& name)
{
    auto actor = std::make_unique<Actor>(name);
    Actor* ptr = actor.get();
    actors.push_back(std::move(actor));
    return ptr;
}

void World::DeleteActor(Actor* actor)
{
    actors.erase(std::remove_if(actors.begin(), actors.end(),
    [actor](const std::unique_ptr<Actor>& a)
        {
            return a.get() == actor;
        }),
        actors.end());
}

void World::Update()
{
    float currentFrame = static_cast<float>(glfwGetTime());
    deletaTime = currentFrame - lastTime;
    lastTime = deletaTime;
    for(auto& actor:actors)
    {
        actor->Update();
    }

}

float World::deletaTime = 0.0f;