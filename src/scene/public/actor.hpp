#pragma once
#include <memory>
#include <vector>
#include <string>
#include "component.hpp"

class Actor
{
    public:
        Actor(const std::string& n="Actor"):name(n){};
    template<typename T, typename... Args>
        T* AddComponent(Args&&... args);
    template<typename T>
        T* GetComponent();
    template<typename T>
        std::vector<T*> GetComponents();

    private:
        std::vector<std::unique_ptr<IComponent>> components;
        std::string name;
};