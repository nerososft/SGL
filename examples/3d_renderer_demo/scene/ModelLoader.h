//
// Created by neo on 25-4-27.
//

#ifndef MODELLOADER_H
#define MODELLOADER_H
#include <assimp/scene.h>
#include <memory>
#include <string>
#include <vector>

#include "Scene.h"

class ModelLoader {
  static aiMatrix4x4 GetNodeTransform(const aiNode *node);

  static aiMatrix4x4 GetMeshTransform(const aiScene *scene,
                                      unsigned int meshIndex);

public:
  ModelLoader() = default;

  ~ModelLoader() = default;

  static std::vector<std::shared_ptr<Mesh>> LoadModel(const std::string &path);
};

#endif // MODELLOADER_H
