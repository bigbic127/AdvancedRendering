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
        const std::map<std::string, std::unique_ptr<IMesh>>& GetMeshes()const{return meshes;}
        const std::map<std::string, std::unique_ptr<IMaterial>>& GetMaterials()const{return materials;}
        IMesh* FindMesh(const std::string name);
        IShader* FindShader(const std::string name);
        ITexture* FindTexture(const std::string name);
        IMaterial* FindMaterial(const std::string name);
    private:
        std::map<std::string, std::unique_ptr<IMesh>> meshes;
        std::map<std::string, std::unique_ptr<IShader>> shaders;
        std::map<std::string, std::unique_ptr<ITexture>> textures;
        std::map<std::string, std::unique_ptr<IMaterial>> materials;
};