//
// Created by neo on 25-4-27.
//

#ifndef MODELLOADER_H
#define MODELLOADER_H
#include <memory>
#include <string>

#include "Mesh.h"

class ModelLoader {
public:
    ModelLoader() = default;

    ~ModelLoader() = default;

    static std::vector<std::shared_ptr<Mesh>> LoadModel(const std::string &path);
};


#endif //MODELLOADER_H
