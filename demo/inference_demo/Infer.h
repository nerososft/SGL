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
#include "engine/ml/MLEngine.h"


class Infer {
    std::shared_ptr<Config> config = nullptr;
    std::shared_ptr<SafeTensor> safeTensor = nullptr;
    std::shared_ptr<Tokenizer> tokenizer = nullptr;
    std::shared_ptr<Model> model = nullptr;
    std::shared_ptr<MLEngine> mle = nullptr;

public:
    Infer() = default;

    ~Infer() = default;

    bool Init();

    void Run(const std::string &prompt) const;
};


#endif //INFER_H
