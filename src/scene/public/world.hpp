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
        void SetLightActor(Actor* light){lightActors.push_back(light);}
        std::vector<Actor*> GetLightActors() const{return lightActors;}
        void SetSelectedActor(Actor* actor){selectedActor = actor;}
        Actor* GetSelectedActor()const{return selectedActor;}
        void Update();
    private:
        std::vector<std::unique_ptr<Actor>> actors;
        std::vector<Actor*> lightActors;
        Actor* currentCamera;
        Actor* selectedActor;
        static float deletaTime;
        float lastTime = 0.0f;
};