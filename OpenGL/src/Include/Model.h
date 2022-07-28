#pragma once
#include <string>
#include <vector>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "Shader.h"
#include "Mesh.h"

class Model
{
public:
    //���ع�������
    std::vector<Texture> textures_loaded;
    /*  ģ������  */
    std::vector<Mesh> meshes;
    std::string directory;
    /*  ����   */
    Model(const std::string& path)
    {
        loadModel(path);
    }
    void Draw(Shader &shader);
private:
    /*  ����   */
    void loadModel(std::string path);
    void processNode(aiNode* node, const aiScene* scene);
    Mesh processMesh(aiMesh* mesh, const aiScene* scene);
    std::vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type,
        std::string typeName);

    unsigned int TextureFromFile(const char* path, const std::string& directory, bool gamma);
};
