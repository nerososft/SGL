//
// Created by neo on 25-4-27.
//

#include "Model.h"

Model::Model(const std::vector<Vertex> &vertices,
             const std::vector<uint32_t> &indices) {
    this->vertices = vertices;
    this->indices = indices;
}
