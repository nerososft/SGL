#include <iostream>
#include <ostream>
#include <glm/fwd.hpp>

#include "gpu_engine/log/Log.h"
#include "ml_engine/MLEngine.h"
#include "ml_engine/operators/IOperator.h"
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

    const std::vector<float> input = {
        -1, 0, 0.4, 0.3, 0.2, 0.1, -0.7, 0.3, 0.2, 0.1, -0.7, 0.3, 0.3, 0.2, 0.1, -0.7,
        -1, 0, 0.4, 0.3, 0.2, 0.1, -0.7, 0.3, 0.2, 0.1, -0.7, 0.3, 0.3, 0.2, 0.1, -0.7,
        -1, 0, 0.4, 0.3, 0.2, 0.1, -0.7, 0.3, 0.2, 0.1, -0.7, 0.3, 0.3, 0.2, 0.1, -0.7,
        -1, 0, 0.4, 0.3, 0.2, 0.1, -0.7, 0.3, 0.2, 0.1, -0.7, 0.3, 0.3, 0.2, 0.1, -0.7,
        -1, 0, 0.4, 0.3, 0.2, 0.1, -0.7, 0.3, 0.2, 0.1, -0.7, 0.3, 0.3, 0.2, 0.1, -0.7,
        -1, 0, 0.4, 0.3, 0.2, 0.1, -0.7, 0.3, 0.2, 0.1, -0.7, 0.3, 0.3, 0.2, 0.1, -0.7,
        -1, 0, 0.4, 0.3, 0.2, 0.1, -0.7, 0.3, 0.2, 0.1, -0.7, 0.3, 0.3, 0.2, 0.1, -0.7,
        -1, 0, 0.4, 0.3, 0.2, 0.1, -0.7, 0.3, 0.2, 0.1, -0.7, 0.3, 0.3, 0.2, 0.1, -0.7,
        -1, 0, 0.4, 0.3, 0.2, 0.1, -0.7, 0.3, 0.2, 0.1, -0.7, 0.3, 0.3, 0.2, 0.1, -0.7,
        -1, 0, 0.4, 0.3, 0.2, 0.1, -0.7, 0.3, 0.2, 0.1, -0.7, 0.3, 0.3, 0.2, 0.1, -0.7,
        -1, 0, 0.4, 0.3, 0.2, 0.1, -0.7, 0.3, 0.2, 0.1, -0.7, 0.3, 0.3, 0.2, 0.1, -0.7,
        -1, 0, 0.4, 0.3, 0.2, 0.1, -0.7, 0.3, 0.2, 0.1, -0.7, 0.3, 0.3, 0.2, 0.1, -0.7,
        -1, 0, 0.4, 0.3, 0.2, 0.1, -0.7, 0.3, 0.2, 0.1, -0.7, 0.3, 0.3, 0.2, 0.1, -0.7,
        -1, 0, 0.4, 0.3, 0.2, 0.1, -0.7, 0.3, 0.2, 0.1, -0.7, 0.3, 0.3, 0.2, 0.1, -0.7,
        -1, 0, 0.4, 0.3, 0.2, 0.1, -0.7, 0.3, 0.2, 0.1, -0.7, 0.3, 0.3, 0.2, 0.1, -0.7,
        -1, 0, 0.4, 0.3, 0.2, 0.1, -0.7, 0.3, 0.2, 0.1, -0.7, 0.3, 0.3, 0.2, 0.1, -0.7,
    };
    const auto reluInputMat = mle.CreateMatrix(16, 16, input);
    const auto reluOutputMat = mle.CreateMatrix(16, 16);
    mle.ReLU(reluInputMat, reluOutputMat);

    // const std::vector<float> mat1 = {
    //     -1, 0, 0.4, 0.3,
    //     0.2, 0.1, -0.7, 0.3,
    //     0.2, 0.1, -0.7, 0.3,
    //     0.3, 0.2, 0.1, -0.7,
    // };
    // const std::vector<float> mat2 = {
    //     -1, 0, 0.4, 0.3,
    //     0.2, 0.1, -0.7, 0.3,
    //     0.2, 0.1, -0.7, 0.3,
    //     0.3, 0.2, 0.1, -0.7,
    // };
    // const std::vector<float> matOutput(16);
    //
    // const auto gemmInputMat1 = mle.CreateMatrix(4, 4, mat1);
    // const auto gemmInputMat2 = mle.CreateMatrix(4, 4, mat2);
    // const auto gemmOutputMat = mle.CreateMatrix(4, 4);
    //
    // mle.MatMul(gemmInputMat1, gemmInputMat2, gemmOutputMat);

    mle.Compute();

    const float *reluData = static_cast<float *>(reluOutputMat->GetDataAddr());
    for (size_t i = 0; i < 256; ++i) {
        std::cout << reluData[i] << " ";
    }
    // std::cout << std::endl;
    // const float *matData = static_cast<float *>(gemmOutputMat->GetDataAddr());
    // for (size_t i = 0; i < 16; ++i) {
    //     std::cout << matData[i] << " ";
    // }
}
