//
// Created by neo on 25-5-7.
//
#include <iostream>

#include "Infer.h"
#include "core/log/Log.h"

int main(int argc, char *argv[]) {
    std::cout << "Inference demo" << std::endl;

    Infer infer;
    infer.Init();
    infer.Run("What is the capital of France?");

    return 0;
}
