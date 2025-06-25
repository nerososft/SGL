//
// Created by neo on 25-6-24.
//

#ifndef INFER_H
#define INFER_H
#include <memory>

#include "Config.h"
#include "Model.h"
#include "SafeTensor.h"
#include "Tokenizer.h"


class Infer {
    std::shared_ptr<Config> config;
    std::shared_ptr<SafeTensor> safeTensor;
    std::shared_ptr<Tokenizer> tokenizer;
    std::shared_ptr<Model> model;

public:
    Infer() = default;

    ~Infer() = default;

    void Init();

    void Run(const std::string &prompt) const;
};


#endif //INFER_H
