#pragma once

class IComponent
{
    public:
        virtual ~IComponent() = default;
        virtual void Update() = 0;
        virtual void Draw() = 0;
};