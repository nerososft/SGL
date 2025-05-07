#include <iostream>
#include <ostream>

#include "gpu_engine/log/Log.h"
#include "ml_engine/MLEngine.h"
#include "ml_engine/operators/IOperator.h"
#include "ml_engine/operators/impl/ReLUOperator.h"
//
// Created by neo on 25-5-7.
//
int main(int argc, char *argv[]) {
    std::cout << "Inference demo" << std::endl;

    MLEngine mle;
    if (!mle.Init()) {
        std::cerr << "Failed to initialize engine" << std::endl;
        return EXIT_FAILURE;
    }

    std::vector<float> input;
    std::vector<float> output;

    mle.ReLU(input, output).Compute();
}
