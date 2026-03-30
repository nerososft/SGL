//
// Created by neo on 25-6-12.
//

#ifndef GUASSIANMODEL_H
#define GUASSIANMODEL_H
#include <glm/vec2.hpp>
#include <glm/vec4.hpp>

struct GaussianPoint {
  glm::vec4 position;
  glm::vec4 color;
  glm::vec4 scale;
  glm::vec4 rotate;
  glm::vec4 opacity;
  // Spherical Harmonics
};

struct GaussianSplatVertex {
  glm::vec4 position;
  glm::vec4 color;
  glm::vec4 scale;
  glm::vec4 rotate;
  glm::vec4 opacity;
  glm::vec2 quadCoord;
};

#endif // GUASSIANMODEL_H
