//
// Created by neo on 25-4-27.
//

#ifndef MODELLOADER_H
#define MODELLOADER_H
#include <memory>
#include <string>

#include "Model.h"


class ModelLoader {
public:
    ModelLoader() = default;

    ~ModelLoader() = default;

    static std::shared_ptr<Model> LoadModel(const std::shared_ptr<VkGPUContext> &gpuCtx,
                                            const std::string &path);
};


#endif //MODELLOADER_H
