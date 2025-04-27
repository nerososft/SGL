//
// Created by neo on 25-4-27.
//

#include "ModelLoader.h"

#include <ostream>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "gpu_engine/log/Log.h"

std::shared_ptr<Model> ModelLoader::LoadModel(const std::string &path) {
    Assimp::Importer importer;
    const aiScene *scene = importer.
            ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);
    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        Logger() << "Failed to load model '" << path << "'" << std::endl;
        return nullptr;
    }

    auto model = std::make_shared<Model>();
    for (int i = 0; i < scene->mNumMeshes; i++) {
        const aiMesh *mesh = scene->mMeshes[i];
        std::cout << "Vertices:" << mesh->mNumVertices << std::endl;
        std::cout << "Faces:" << mesh->mNumFaces << std::endl;
        for (int j = 0; j < mesh->mNumFaces; j++) {
            const aiFace face = mesh->mFaces[j];
            for (int k = 0; k < face.mNumIndices; k++) {
                Vertex vertex{};
                vertex.position.x = (mesh->mVertices[face.mIndices[k]].x - 50.0f) / 100.0f;
                vertex.position.y = (mesh->mVertices[face.mIndices[k]].y - 50.0f) / 100.0f;
                vertex.position.z = (mesh->mVertices[face.mIndices[k]].z - 50.0f) / 100.0f;

                vertex.color.r = rand() % 100 / 100.0f;
                vertex.color.g = rand() % 100 / 100.0f;
                vertex.color.b = rand() % 100 / 100.0f;

                model->vertices.push_back(vertex);
            }
            model->indices.push_back(j * face.mNumIndices + 0);
            model->indices.push_back(j * face.mNumIndices + 1);
            model->indices.push_back(j * face.mNumIndices + 2);
        }
    }
    std::cout << "Loaded model '" << path
            << "', vertex:" << model->vertices.size()
            << ", indices:" << model->indices.size() << std::endl;

    return model;
}
