#include "actor.hpp"

void Actor::Update()
{
    for(auto& component:components)
        component->Update();
}

void Actor::Draw()
{
    for(auto& component:components)
        component->Draw();
}