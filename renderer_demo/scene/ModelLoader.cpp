//
// Created by neo on 25-4-27.
//

#include "ModelLoader.h"

#include <ostream>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "gpu_engine/log/Log.h"

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
    for (int i = 0; i < scene->mNumMeshes; i++) {
        auto model = std::make_shared<Mesh>();
        const aiMesh *mesh = scene->mMeshes[i];
        Logger() << "Loading mesh '" << mesh->mName.C_Str() <<
                "', Vertices:" << mesh->mNumVertices <<
                ", Faces:" << mesh->mNumFaces << std::endl;
        for (int j = 0; j < mesh->mNumFaces; j++) {
            const aiFace face = mesh->mFaces[j];
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
            for (int k = 0; k < face.mNumIndices; k++) {
                Vertex vertex{};
                vertex.position.x = mesh->mVertices[face.mIndices[k]].x;
                vertex.position.y = mesh->mVertices[face.mIndices[k]].y;
                vertex.position.z = mesh->mVertices[face.mIndices[k]].z;

                if (mesh->HasNormals()) {
                    normal = mesh->mNormals[j * face.mNumIndices + k];
                }
                vertex.normal.x = normal.x;
                vertex.normal.y = normal.y;
                vertex.normal.z = normal.z;

                vertex.color.r = 0.0f;
                vertex.color.g = 0.0f;
                vertex.color.b = 0.0f;

                model->vertices.push_back(vertex);
            }

            model->indices.push_back(j * face.mNumIndices + 0);
            model->indices.push_back(j * face.mNumIndices + 1);
            model->indices.push_back(j * face.mNumIndices + 2);
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

            model->material.shininess = shininess;
        }

        meshes.push_back(model);
    }

    return meshes;
}
