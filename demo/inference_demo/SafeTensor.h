//
// Created by neo on 25-6-24.
//

#ifndef SAFETENSORDATA_H
#define SAFETENSORDATA_H
#include <string>

#include "Config.h"
#include "vendor/json.hpp"

class SafeTensor {
    std::shared_ptr<Config> config = nullptr;

public:
    SafeTensor() = default;

    ~SafeTensor() = default;

    bool LoadFromFile(const std::string &tensorFilePath);
};

#endif //SAFETENSORDATA_H
