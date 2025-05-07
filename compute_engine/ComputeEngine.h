//
// Created by neo on 25-5-7.
//

#ifndef COMPUTEENGINE_H
#define COMPUTEENGINE_H
#include "gpu_engine/gpu/compute_graph/ComputeGraph.h"

class ComputeEngine {
    std::shared_ptr<ComputeGraph> computeGraph = nullptr;

public:
    ComputeEngine() = default;

    ~ComputeEngine() = default;
};


#endif //COMPUTEENGINE_H
