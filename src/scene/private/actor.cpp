#include "actor.hpp"

template<typename T, typename... Args>
T* Actor::AddComponent(Args&&... args)
{
    auto component = std::make_unique<T>(std::forward<Args>(args)...);
    T* ptr = component.get();
    components.push_back(std::move(component));
    return ptr;
}

template<typename T>
T* Actor::GetComponent()
{
    for(auto& c:components)
    {
        if(auto casted = dynamic_cast<T*>(c.get()))
            return casted;
    }
    return nullptr;
}

template<typename T>
std::vector<T*> Actor::GetComponents()
{
    std::vector<T*> result;
    for (auto& c:components)
    {
        if(auto casted = dynamic_cast<T*>(c.get()))
            result.push_back(casted);
    }
    return result;                
}