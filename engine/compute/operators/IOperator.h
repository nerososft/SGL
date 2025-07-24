//
// Created by neo on 2025/3/29.
//

#ifndef IOPERATOR_H
#define IOPERATOR_H
#include "core/gpu/compute_graph/IComputeGraphNode.h"

class IOperator {
public:
  IOperator() = default;

  virtual ~IOperator() = default;

  virtual std::shared_ptr<IComputeGraphNode> CreateComputeGraphNode() = 0;

  virtual void Destroy() = 0;
};

#endif // IOPERATOR_
