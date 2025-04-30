//
// Created by neo on 25-4-27.
//

#include "ModelLoader.h"

#include <ostream>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "gpu_engine/log/Log.h"

aiMatrix4x4 ModelLoader::GetNodeTransform(const aiNode *node) {
    if (!node->mParent) {
        return node->mTransformation;
    }
    return GetNodeTransform(node->mParent) * node->mTransformation;
}

aiMatrix4x4 ModelLoader::GetMeshTransform(const aiScene *scene, const unsigned int meshIndex) {
    const aiMesh *mesh = scene->mMeshes[meshIndex];
    const aiNode *node = scene->mRootNode->FindNode(mesh->mName);

    return GetNodeTransform(node);
}

std::vector<std::shared_ptr<Mesh> > ModelLoader::LoadModel(const std::string &path) {
    Assimp::Importer importer;
    const aiScene *scene = importer.
            ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);
    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        Logger() << "Failed to load model '" << path << "'" << std::endl;
        return {};
    }

    std::vector<std::shared_ptr<Mesh> > meshes;
    Logger() << "Loading model '" << path << "'" << std::endl;

    for (int meshIndex = 0; meshIndex < scene->mNumMeshes; meshIndex++) {
        auto model = std::make_shared<Mesh>();
        const aiMesh *mesh = scene->mMeshes[meshIndex];
        Logger() << "Loading mesh '" << mesh->mName.C_Str() <<
                "', Vertices:" << mesh->mNumVertices <<
                ", Faces:" << mesh->mNumFaces << std::endl;
        for (int faceIndex = 0; faceIndex < mesh->mNumFaces; faceIndex++) {
            const aiFace face = mesh->mFaces[faceIndex];
            aiVector3D normal;
            if (!mesh->HasNormals()) {
                aiVector3D v0 = mesh->mVertices[face.mIndices[0]];
                aiVector3D v1 = mesh->mVertices[face.mIndices[1]];
                aiVector3D v2 = mesh->mVertices[face.mIndices[2]];
                aiVector3D edge1 = v1 - v0;
                aiVector3D edge2 = v2 - v0;
                normal = edge1 ^ edge2;
                normal.Normalize();
            }
            for (int faceIndices = 0; faceIndices < face.mNumIndices; faceIndices++) {
                Vertex vertex{};
                vertex.position.x = mesh->mVertices[face.mIndices[faceIndices]].x;
                vertex.position.y = mesh->mVertices[face.mIndices[faceIndices]].y;
                vertex.position.z = mesh->mVertices[face.mIndices[faceIndices]].z;

                if (mesh->HasNormals()) {
                    normal = mesh->mNormals[faceIndex * face.mNumIndices + faceIndices];
                }
                vertex.normal.x = normal.x;
                vertex.normal.y = normal.y;
                vertex.normal.z = normal.z;

                vertex.color.r = 0.5f;
                vertex.color.g = 0.5f;
                vertex.color.b = 0.5f;

                model->vertexData.push_back(vertex);
            }

            model->indicesData.push_back(faceIndex * face.mNumIndices + 0);
            model->indicesData.push_back(faceIndex * face.mNumIndices + 1);
            model->indicesData.push_back(faceIndex * face.mNumIndices + 2);
        }

        if (scene->HasTextures()) {
            aiTexture *texture = scene->mTextures[meshIndex];
            Logger() << "Loading texture '" << texture->mFilename.C_Str() << "'" << std::endl;
        }

        if (scene->HasMaterials()) {
            aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];
            aiColor3D ambientColor, diffuseColor, specularColor, emissiveColor, transparentColor, reflectiveColor;
            float shininess;
            aiString materialName;
            material->Get(AI_MATKEY_NAME, materialName);
            Logger() << "Loading material '" << materialName.C_Str() << "'" << std::endl;

            material->Get(AI_MATKEY_COLOR_AMBIENT, ambientColor);
            material->Get(AI_MATKEY_COLOR_DIFFUSE, diffuseColor);
            material->Get(AI_MATKEY_COLOR_SPECULAR, specularColor);
            material->Get(AI_MATKEY_COLOR_EMISSIVE, emissiveColor);
            material->Get(AI_MATKEY_COLOR_TRANSPARENT, transparentColor);
            material->Get(AI_MATKEY_COLOR_REFLECTIVE, reflectiveColor);
            material->Get(AI_MATKEY_SHININESS, shininess);

            model->material.ambientColor.r = ambientColor.r;
            model->material.ambientColor.g = ambientColor.g;
            model->material.ambientColor.b = ambientColor.b;

            model->material.diffuseColor.r = diffuseColor.r;
            model->material.diffuseColor.g = diffuseColor.g;
            model->material.diffuseColor.b = diffuseColor.b;

            model->material.specularColor.r = specularColor.r;
            model->material.specularColor.g = specularColor.g;
            model->material.specularColor.b = specularColor.b;

            model->material.transparentColor.r = transparentColor.r;
            model->material.transparentColor.g = transparentColor.g;
            model->material.transparentColor.b = transparentColor.b;

            model->material.emissiveColor.r = emissiveColor.r;
            model->material.emissiveColor.g = emissiveColor.g;
            model->material.emissiveColor.b = emissiveColor.b;

            model->material.reflectiveColor.r = reflectiveColor.r;
            model->material.reflectiveColor.g = reflectiveColor.g;
            model->material.reflectiveColor.b = reflectiveColor.b;

            model->material.shininess.r = shininess;

            for (uint32_t textureType = aiTextureType_NONE; textureType < aiTextureType_UNKNOWN; textureType++) {
                uint32_t normalTextureCount = material->GetTextureCount(static_cast<aiTextureType>(textureType));
                Logger() << "Texture " << aiTextureTypeToString(static_cast<aiTextureType>(textureType)) <<
                        " count: " << normalTextureCount <<
                        std::endl;
                for (uint32_t i = 0; i < normalTextureCount; i++) {
                    aiString texturePath;
                    material->GetTexture(static_cast<aiTextureType>(textureType), i, &texturePath);
                    Logger() << "Loading " << aiTextureTypeToString(static_cast<aiTextureType>(textureType)) <<
                            "texture '" << texturePath.C_Str()
                            << "'" << std::endl;
                }
            }
        }

        aiMatrix4x4 matrix = GetMeshTransform(scene, meshIndex);
        model->transform = glm::mat4x4(matrix.a1, matrix.b1, matrix.c1, matrix.d1,
                                       matrix.a2, matrix.b2, matrix.c2, matrix.d2,
                                       matrix.a3, matrix.b3, matrix.c3, matrix.d3,
                                       matrix.a4, matrix.b4, matrix.c4, matrix.d4);

        meshes.push_back(model);
    }

    return meshes;
}
