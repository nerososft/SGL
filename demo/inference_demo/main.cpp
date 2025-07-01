#include <iostream>

#include "Infer.h"
#include "Tokenizer.h"
#include "core/log/Log.h"
#include "core/utils/TimeUtils.h"
#include "engine/ml/MLEngine.h"
//
// Created by neo on 25-5-7.
//
int main(int argc, char *argv[]) {
    std::cout << "Inference demo" << std::endl;

    Infer infer;
    infer.Init();
    infer.Run("What is the capital of France?");

    return 0;
}
