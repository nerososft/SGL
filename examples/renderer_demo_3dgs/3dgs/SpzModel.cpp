//
// Created by neo on 25-6-12.
//

#include "SpzModel.h"

#include "runtime/log/Log.h"

#include <cmath>
#include <glm/vec3.hpp>

namespace {

float DecodeSigned24(const uint8_t val[3], const uint32_t fractionalBits) {
  int32_t fixed = static_cast<int32_t>(val[0]) |
                  (static_cast<int32_t>(val[1]) << 8) |
                  (static_cast<int32_t>(val[2]) << 16);

  if (fixed & 0x800000) {
    fixed |= 0xFF000000;
  }

  return static_cast<float>(fixed) / static_cast<float>(1 << fractionalBits);
}

float DecodeScale(const uint8_t value) {
  return std::exp(static_cast<float>(value) / 16.0f - 10.0f);
}

} // namespace

bool SpzModel::loadModel(const char *str) {
  SpzFile file{};
  this->gaussianPoints.clear();

  this->modelBytes = IOUtils::ReadFile(str);
  if (this->modelBytes.size() == 0) {
    Logger() << "No data in file!" << std::endl;
    return false;
  }

  Logger() << "Loaded Model from File: " << str
           << ", size=" << modelBytes.size() << std::endl;
  char *buffer = modelBytes.data();
  auto *header = reinterpret_cast<SpzFileHeader *>(buffer);
  if (header->magic != SPZ_HEADER_MAGIC) {
    Logger() << "Model is not a valid PackedGaussian header!" << std::endl;
    return false;
  }
  Logger() << "Magic: " << header->magic << std::endl;
  Logger() << "Version: " << header->version << std::endl;
  Logger() << "NumberPoints: " << header->numPoints << std::endl;
  Logger() << "FractionalBits: "
           << static_cast<uint32_t>(header->fractionalBits) << std::endl;
  Logger() << "Flags: " << static_cast<uint32_t>(header->flags) << std::endl;
  file.header = header;
  file.positions =
      reinterpret_cast<SpzFilePosition *>(buffer + sizeof(SpzFileHeader));
  const size_t numPoints = header->numPoints;
  const size_t positionsSize = sizeof(SpzFilePosition) * numPoints;
  const size_t alphasSize = sizeof(SpzFileAlpha) * numPoints;
  const size_t colorsSize = sizeof(SpzFileColor) * numPoints;
  const size_t scalesSize = sizeof(SpzFileScale) * numPoints;
  const size_t rotationStride = header->version >= 2 ? 3 : 4;
  const size_t rotationsSize = rotationStride * numPoints;
  const size_t requiredBytes = sizeof(SpzFileHeader) + positionsSize +
                               alphasSize + colorsSize + scalesSize +
                               rotationsSize;
  if (this->modelBytes.size() < requiredBytes) {
    Logger() << Logger::ERROR << "SPZ file is truncated" << std::endl;
    return false;
  }

  file.alphas =
      reinterpret_cast<SpzFileAlpha *>(buffer + sizeof(SpzFileHeader) +
                                       positionsSize);
  file.colors = reinterpret_cast<SpzFileColor *>(
      reinterpret_cast<char *>(file.alphas) + alphasSize);
  file.scales = reinterpret_cast<SpzFileScale *>(
      reinterpret_cast<char *>(file.colors) + colorsSize);
  file.rotations = reinterpret_cast<SpzFileRotation *>(
      reinterpret_cast<char *>(file.scales) + scalesSize);

  this->gaussianPoints.reserve(numPoints);
  for (size_t i = 0; i < numPoints; i++) {
    GaussianPoint point{};
    auto [p_x, p_y, p_z] = file.positions[i];
    point.position.x = DecodeSigned24(p_x, header->fractionalBits);
    point.position.y = DecodeSigned24(p_y, header->fractionalBits);
    point.position.z = DecodeSigned24(p_z, header->fractionalBits);
    point.position.w = 1.0f;

    point.scale.x = DecodeScale(file.scales[i].x);
    point.scale.y = DecodeScale(file.scales[i].y);
    point.scale.z = DecodeScale(file.scales[i].z);

    const float alpha = static_cast<float>(file.alphas[i].x) / 255.0f;
    point.opacity = glm::vec4(alpha);
    point.color.x = static_cast<float>(file.colors[i].x) / 255.0f;
    point.color.y = static_cast<float>(file.colors[i].y) / 255.0f;
    point.color.z = static_cast<float>(file.colors[i].z) / 255.0f;
    point.color.w = alpha;
    point.rotate = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
    this->gaussianPoints.push_back(point);
  }

  return true;
}
