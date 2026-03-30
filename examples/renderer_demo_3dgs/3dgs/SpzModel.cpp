//
// Created by neo on 25-6-12.
//

#include "SpzModel.h"

#include "runtime/log/Log.h"

#include <algorithm>
#include <cmath>
#include <glm/geometric.hpp>
#include <glm/vec4.hpp>
#include <glm/vec3.hpp>

namespace {
constexpr float kColorScale = 0.15f;
constexpr float kSqrtHalf = 0.7071067811865475244f;

uint32_t SHCoeffCountForDegree(const uint32_t degree) {
  switch (degree) {
  case 0:
    return 0;
  case 1:
    return 3;
  case 2:
    return 8;
  case 3:
    return 15;
  case 4:
    return 24;
  default:
    return 0;
  }
}

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

float DecodeOpacity(const uint8_t value) {
  const float alpha =
      std::clamp(static_cast<float>(value) / 255.0f, 1e-6f, 1.0f - 1e-6f);
  return std::log(alpha / (1.0f - alpha));
}

float DecodeColorDC(const uint8_t value) {
  return (static_cast<float>(value) / 255.0f - 0.5f) / kColorScale;
}

float DecodeSH(const uint8_t value) {
  return (static_cast<float>(value) - 128.0f) / 128.0f;
}

glm::vec4 DecodeRotationV2(const uint8_t *rotationData) {
  const glm::vec3 xyz(
      static_cast<float>(rotationData[0]) / 127.5f - 1.0f,
      static_cast<float>(rotationData[1]) / 127.5f - 1.0f,
      static_cast<float>(rotationData[2]) / 127.5f - 1.0f);
  const float w = std::sqrt(std::max(0.0f, 1.0f - glm::dot(xyz, xyz)));
  return glm::normalize(glm::vec4(xyz, w));
}

glm::vec4 DecodeRotationV3(const uint8_t *rotationData) {
  uint32_t comp = static_cast<uint32_t>(rotationData[0]) |
                  (static_cast<uint32_t>(rotationData[1]) << 8) |
                  (static_cast<uint32_t>(rotationData[2]) << 16) |
                  (static_cast<uint32_t>(rotationData[3]) << 24);
  constexpr uint32_t kMask = (1u << 9u) - 1u;
  const uint32_t largestIndex = comp >> 30u;
  glm::vec4 rotation(0.0f);
  float sumSquares = 0.0f;
  for (int i = 3; i >= 0; --i) {
    if (static_cast<uint32_t>(i) == largestIndex) {
      continue;
    }
    const uint32_t magnitude = comp & kMask;
    const uint32_t negativeBit = (comp >> 9u) & 0x1u;
    comp >>= 10u;
    float value = kSqrtHalf * static_cast<float>(magnitude) /
                  static_cast<float>(kMask);
    if (negativeBit != 0u) {
      value = -value;
    }
    rotation[i] = value;
    sumSquares += value * value;
  }
  rotation[largestIndex] = std::sqrt(std::max(0.0f, 1.0f - sumSquares));
  return rotation;
}

glm::vec4 DecodeRotation(const uint8_t *rotationData, const uint32_t version) {
  if (version >= 3) {
    return DecodeRotationV3(rotationData);
  }
  return DecodeRotationV2(rotationData);
}

} // namespace

bool SpzModel::loadModel(const char *str) {
  SpzFile file{};
  this->gaussianPoints.clear();
  this->shCoefficients.clear();

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
  const size_t rotationStride = header->version >= 3 ? 4 : 3;
  const size_t rotationsSize = rotationStride * numPoints;
  const size_t shCoeffCount = SHCoeffCountForDegree(header->shDegree);
  const size_t shSize = shCoeffCount * 3 * numPoints;
  const size_t requiredBytes = sizeof(SpzFileHeader) + positionsSize +
                               alphasSize + colorsSize + scalesSize +
                               rotationsSize + shSize;
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
  file.rotations = reinterpret_cast<uint8_t *>(
      reinterpret_cast<char *>(file.scales) + scalesSize);
  file.sphericalHarmonics = reinterpret_cast<SpzFileSphericalHarmonic9 *>(
      file.rotations + rotationsSize);

  this->gaussianPoints.reserve(numPoints);
  this->shCoefficients.assign(numPoints * SPZ_RENDERED_SH_COEFF_COUNT,
                              glm::vec4(0.0f));
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

    point.opacity = glm::vec4(DecodeOpacity(file.alphas[i].x));
    point.color.x = DecodeColorDC(file.colors[i].x);
    point.color.y = DecodeColorDC(file.colors[i].y);
    point.color.z = DecodeColorDC(file.colors[i].z);
    point.color.w = 1.0f;
    point.rotate =
        DecodeRotation(file.rotations + i * rotationStride, header->version);

    const uint8_t *packedSH =
        reinterpret_cast<const uint8_t *>(file.sphericalHarmonics) +
        i * shCoeffCount * 3;
    const size_t renderedCoeffCount =
        std::min<size_t>(SPZ_RENDERED_SH_COEFF_COUNT, shCoeffCount);
    for (size_t coeffIdx = 0; coeffIdx < renderedCoeffCount; ++coeffIdx) {
      glm::vec4 coeff(0.0f);
      coeff.x = DecodeSH(packedSH[coeffIdx * 3 + 0]);
      coeff.y = DecodeSH(packedSH[coeffIdx * 3 + 1]);
      coeff.z = DecodeSH(packedSH[coeffIdx * 3 + 2]);
      this->shCoefficients[i * SPZ_RENDERED_SH_COEFF_COUNT + coeffIdx] = coeff;
    }
    this->gaussianPoints.push_back(point);
  }

  return true;
}
