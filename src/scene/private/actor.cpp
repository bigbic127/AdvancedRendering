#include "actor.hpp"

void Actor::Update()
{
    for(auto& component:components)
    {
        component->Update();
        component->Draw();
    }
}