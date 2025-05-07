//
// Created by neo on 25-5-7.
//

#ifndef MLENGINE_H
#define MLENGINE_H
#include <memory>

#include "gpu_engine/gpu/VkGPUContext.h"
#include "gpu_engine/gpu/compute_graph/ComputeGraph.h"

class MLEngine {
    std::shared_ptr<VkGPUContext> gpuCtx = nullptr;
    std::shared_ptr<ComputeGraph> graph = nullptr;
    std::shared_ptr<SubComputeGraph> mainSubGraph = nullptr;

public:
    MLEngine() = default;

    bool Init();

    MLEngine ReLU(std::vector<float> input, std::vector<float> output);

    ~MLEngine() = default;

    void Compute() const;
};


#endif //MLENGINE_H
