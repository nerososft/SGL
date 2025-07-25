//
// Created by neo on 25-7-3.
//

#ifndef SEQUENCE_H
#define SEQUENCE_H
#include "runtime/gpu/compute_graph/ComputeGraph.h"

class Sequence : public std::enable_shared_from_this<Sequence> {
  std::shared_ptr<VkGPUContext> gpuCtx = nullptr;
  std::shared_ptr<ComputeGraph> graph = nullptr;
  std::shared_ptr<SubComputeGraph> mainSubGraph = nullptr;

public:
  explicit Sequence(const std::shared_ptr<VkGPUContext> &gpuCtx);

  bool Init();

  ~Sequence() = default;

  std::shared_ptr<Sequence>
  Record(const std::shared_ptr<IComputeGraphNode> &node);

  std::shared_ptr<Sequence> Eval();

  std::shared_ptr<Sequence> Destroy();
};

#endif // SEQUENCE_H
