#pragma once
#include "actor.hpp"
#include <string>
#include <vector>
#include <map>

class World
{
    public:
        Actor* CreateActor(const std::string& name = "Actor");
        void DeleteActor(Actor* actor);
        static float GetDeletaTime(){return deletaTime;}
        void Update();
    private:
        std::vector<std::unique_ptr<Actor>> actors;
        static float deletaTime;
        float lastTime = 0.0f;
};