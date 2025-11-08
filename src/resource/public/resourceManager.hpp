#pragma once
#include "mesh.hpp"
#include "shader.hpp"
#include "texture.hpp"
#include "material.hpp"
#include <memory>
#include <map>
#include <string>

class ResourceManager
{
    public:
        ResourceManager();
        const std::map<std::string, std::unique_ptr<IMesh>>& GetStandardMeshes()const{return meshes;}
    private:
        std::map<std::string, std::unique_ptr<IMesh>> meshes;
        std::map<std::string, std::unique_ptr<IShader>> shaders;
        std::map<std::string, std::unique_ptr<ITexture>> textures;
        std::map<std::string, std::unique_ptr<IMaterial>> materials;
};