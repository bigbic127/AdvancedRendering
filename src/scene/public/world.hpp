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
        const std::vector<std::unique_ptr<Actor>>& GetActors() const{return actors;}
        void SetCurrentCamera(Actor* camera){currentCamera = camera;}
        Actor* GetCurrentCamera()const{return currentCamera;}
        void SetCurrentLight(Actor* light){currentLight = light;}
        Actor* GetCurrentLight() const{return currentLight;}
        void SetSelectedActor(Actor* actor){currentSelectedActor = actor;}
        Actor* GetSelectedActor()const{return currentSelectedActor;}
        void Update();
    private:
        std::vector<std::unique_ptr<Actor>> actors;
        Actor* currentLight;
        Actor* currentCamera;
        Actor* currentSelectedActor;
        static float deletaTime;
        float lastTime = 0.0f;
};