//
// Created by neo on 25-6-12.
//

#include "SpzModel.h"

#include "core/log/Log.h"

float decodeLog(const uint8_t value, const float min, const float max) {
    const float normalized = value / 255.0f;
    const float log_val = normalized * (max - min);
    return min * powf(2.0f, log_val);
}

bool SpzModel::loadModel(const char *str) {
    SpzFile file{};

    this->modelBytes = IOUtils::ReadFile(str);

    Logger() << "Loaded Model from File: " << str << ", size=" << modelBytes.size() << std::endl;
    char *buffer = modelBytes.data();
    auto *header = reinterpret_cast<SpzFileHeader *>(buffer);
    if (header->magic != SPZ_HEADER_MAGIC) {
        Logger() << "Model is not a valid PackedGaussian header!" << std::endl;
        return false;
    }
    Logger() << "Magic: " << header->magic << std::endl;
    Logger() << "Version: " << header->version << std::endl;
    Logger() << "NumberPoints: " << header->numPoints << std::endl;
    Logger() << "FractionalBits: " << static_cast<uint32_t>(header->fractionalBits) << std::endl;
    Logger() << "Flags: " << static_cast<uint32_t>(header->flags) << std::endl;
    file.header = header;
    file.positions = reinterpret_cast<SpzFilePosition *>(buffer + sizeof(SpzFileHeader));
    const auto denominator = static_cast<float>(1 << header->fractionalBits);
    file.scales = reinterpret_cast<SpzFileScale *>(file.positions + sizeof(SpzFilePosition) * header->numPoints);
    for (size_t i = 0; i < header->numPoints; i++) {
        GaussianPoint point{};
        auto [p_x, p_y, p_z] = file.positions[i];
        point.position.x = static_cast<float>(p_x[0] << 16 | p_x[1] << 8 | p_x[2]) / denominator;
        point.position.y = static_cast<float>(p_y[0] << 16 | p_y[1] << 8 | p_y[2]) / denominator;
        point.position.z = static_cast<float>(p_z[0] << 16 | p_z[1] << 8 | p_z[2]) / denominator;

        auto [s_x, s_y, s_z] = file.scales[i];
        std::cout << s_x << ", " << s_y << ", " << s_z << std::endl;

        this->gaussianPoints.push_back(point);
    }

    return true;
}
