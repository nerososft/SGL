//
// Created by neo on 25-4-27.
//

#include "ModelLoader.h"

#include <ostream>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "gpu_engine/log/Log.h"

std::shared_ptr<Model> ModelLoader::LoadModel(const std::shared_ptr<VkGPUContext>& gpuCtx, const std::string &path) {
    Assimp::Importer importer;
    const aiScene *scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);
    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        Logger() << "Failed to load model '" << path << "'" << std::endl;
        return nullptr;
    }
    for (int i = 0; i < scene->mNumMeshes; i++) {
        const aiMesh *mesh = scene->mMeshes[i];
        for (int j = 0; j < mesh->mNumVertices; j++) {
            const aiVector3D vertex = mesh->mVertices[j];
            std::cout << vertex.x << ", " << vertex.y << ", " << vertex.z << std::endl;
        }
        for (int j = 0; j < mesh->mNumFaces; j++) {
            aiFace face = mesh->mFaces[j];
        }
    }

    std::shared_ptr<VkGPUBuffer> vertexBuffer = std::make_shared<VkGPUBuffer>(gpuCtx);
    // vertexBuffer->AllocateAndBind(GPU_BUFFER_TYPE_STORAGE_SHARED, )
    return std::make_shared<Model>();
}
