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

    const std::vector<float> input = {-1, 0, 0.4, 0.3, 0.2, 0.1, -0.7, 0.3, 0.2, 0.1};
    std::vector<float> output(input.size());

    mle.ReLU(input, output).Compute();

    std::cout << output.data() << std::endl;
}
