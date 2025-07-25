//
// Created by neo on 25-6-12.
//

#include "SpzModel.h"

#include "core/log/Log.h"

float float24(const uint8_t val[3], const uint32_t fractionalBits) {
  int32_t fixed = (static_cast<int32_t>(val[0]) << 16) |
                  (static_cast<int32_t>(val[1]) << 8) |
                  static_cast<int32_t>(val[2]);

  if (fixed & 0x800000) {
    fixed |= 0xFF000000;
  }

  return static_cast<float>(fixed) / static_cast<float>(1 << fractionalBits);
}

float decodeLog(const uint8_t value, const float min, const float max) {
  const float normalized = static_cast<float>(value) / 255.0f;
  const float log_val = normalized * log2(max / min);
  return min * powf(2.0f, log_val);
}

bool SpzModel::loadModel(const char *str) {
  SpzFile file{};

  this->modelBytes = IOUtils::ReadFile(str);

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
  const auto denominator = static_cast<float>(2 << header->fractionalBits);
  file.scales = reinterpret_cast<SpzFileScale *>(
      buffer + sizeof(SpzFileHeader) +
      sizeof(SpzFilePosition) * header->numPoints);

  auto scaleMin = glm::vec3(MAXFLOAT);
  auto scaleMax = glm::vec3(0.0f);
  for (size_t i = 0; i < header->numPoints; i++) {
    GaussianPoint point{};
    auto [p_x, p_y, p_z] = file.positions[i];
    point.position.x = float24(p_x, header->fractionalBits);
    point.position.y = float24(p_y, header->fractionalBits);
    point.position.z = float24(p_z, header->fractionalBits);

    auto [s_x, s_y, s_z] = file.scales[i];
    if (static_cast<float>(s_x) <= scaleMin.x) {
      scaleMin.x = s_x;
    }
    if (static_cast<float>(s_y) <= scaleMin.y) {
      scaleMin.y = s_y;
    }
    if (static_cast<float>(s_z) <= scaleMin.z) {
      scaleMin.z = s_z;
    }
    if (static_cast<float>(s_x) >= scaleMax.x) {
      scaleMax.x = s_x;
    }
    if (static_cast<float>(s_y) >= scaleMax.y) {
      scaleMax.y = s_y;
    }
    if (static_cast<float>(s_z) >= scaleMax.z) {
      scaleMax.z = s_z;
    }
    this->gaussianPoints.push_back(point);
  }

  file.rotations = reinterpret_cast<SpzFileRotation *>(
      buffer + sizeof(SpzFileHeader) +
      sizeof(SpzFilePosition) * header->numPoints +
      sizeof(SpzFileScale) * header->numPoints);

  file.alphas = reinterpret_cast<SpzFileAlpha *>(
      buffer + sizeof(SpzFileHeader) +
      sizeof(SpzFilePosition) * header->numPoints +
      sizeof(SpzFileScale) * header->numPoints +
      sizeof(SpzFileRotation) * header->numPoints);

  file.colors = reinterpret_cast<SpzFileColor *>(
      buffer + sizeof(SpzFileHeader) +
      sizeof(SpzFilePosition) * header->numPoints +
      sizeof(SpzFileScale) * header->numPoints +
      sizeof(SpzFileRotation) * header->numPoints +
      sizeof(SpzFileAlpha) * header->numPoints);
  for (size_t i = 0; i < header->numPoints; i++) {
    this->gaussianPoints[i].scale.x =
        decodeLog(file.scales[i].x, scaleMin.x, scaleMax.x);
    this->gaussianPoints[i].scale.y =
        decodeLog(file.scales[i].y, scaleMin.y, scaleMax.y);
    this->gaussianPoints[i].scale.z =
        decodeLog(file.scales[i].z, scaleMin.z, scaleMax.z);

    this->gaussianPoints[i].rotate.x = file.rotations[i].x;
    this->gaussianPoints[i].rotate.y = file.rotations[i].y;
    this->gaussianPoints[i].rotate.z = file.rotations[i].z;

    this->gaussianPoints[i].opacity.x =
        static_cast<float>(file.alphas[i].x) / 255.0f;

    this->gaussianPoints[i].color.x =
        static_cast<float>(file.colors[i].x) / 255.0f;
    this->gaussianPoints[i].color.y =
        static_cast<float>(file.colors[i].y) / 255.0f;
    this->gaussianPoints[i].color.z =
        static_cast<float>(file.colors[i].z) / 255.0f;
    this->gaussianPoints[i].color.a =
        static_cast<float>(file.alphas[i].x) / 255.0f;
  }

  return true;
}
