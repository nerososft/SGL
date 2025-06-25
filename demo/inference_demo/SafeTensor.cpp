//
// Created by neo on 25-6-24.
//

#include "SafeTensor.h"

#include <fstream>

#include "core/log/Log.h"
#include "core/utils/IOUtils.h"
#include "vendor/json.hpp"

float BF16ToFP32(const uint16_t bf16) {
    union {
        uint32_t u32;
        float f32;
    } converter;

    // BF16 结构: [1位符号][8位指数][7位尾数]
    // FP32 结构: [1位符号][8位指数][23位尾数]
    converter.u32 = static_cast<uint32_t>(bf16) << 16;
    return converter.f32;
}

bool SafeTensor::LoadBF16EmbeddingMatrix(const std::vector<char> &safeTensorData,
                                         nlohmann::json::const_reference embeddingMatrixObj) {
    Logger() << "Embedding matrix loading..." << std::endl;

    std::cout << embeddingMatrixObj << std::endl;

    if (!embeddingMatrixObj.is_object()) {
        return false;
    }
    if (!embeddingMatrixObj["data_offsets"].is_array()) {
        return false;
    }
    if (!embeddingMatrixObj["shape"].is_array()) {
        return false;
    }

    const uint64_t dataBegin = embeddingMatrixObj["data_offsets"][0];
    const uint64_t dataEnd = embeddingMatrixObj["data_offsets"][1];
    std::string dataType = embeddingMatrixObj["dtype"];
    const uint64_t shapeHeight = embeddingMatrixObj["shape"][0];
    const uint64_t shapeWidth = embeddingMatrixObj["shape"][1];
    const uint64_t dataSize = dataEnd - dataBegin;

    Logger() << "Embedding matrix range: (" << dataBegin << ", " << dataEnd << ")" << std::endl;
    Logger() << "Embedding matrix shape: (" << shapeHeight << ", " << shapeWidth << ")" << std::endl;
    Logger() << "Embedding matrix data size: " << dataSize << std::endl;

    const uint16_t *pf16Data = (uint16_t *) (safeTensorData.data() + dataBegin);
    this->embeddingMatrix.resize(shapeHeight);
    for (uint64_t i = 0; i < shapeHeight; i++) {
        this->embeddingMatrix[i].resize(shapeWidth);
        for (uint64_t j = 0; j < shapeWidth; j++) {
            this->embeddingMatrix[i][j] = BF16ToFP32(pf16Data[i * shapeWidth + j]);
        }
    }

    return true;
}

bool SafeTensor::LoadFromFile(const std::string &tensorFilePath) {
    const std::vector<char> bytes = IOUtils::ReadFile(tensorFilePath);
    if (bytes.empty()) {
        Logger() << "Failed '" << tensorFilePath << "' is empty" << std::endl;
    }
    const char *data = bytes.data();
    if (data == nullptr) {
        Logger() << "Failed '" << tensorFilePath << "' is empty" << std::endl;
        return false;
    }
    const uint64_t headSize = ((uint64_t *) (data))[0];
    Logger() << "Head size: " << headSize << std::endl;

    auto headerJsonStr = std::string(data + 8, headSize);
    const nlohmann::json header = nlohmann::json::parse(headerJsonStr);

    const nlohmann::json embeddingMatrixObj = header["model.embed_tokens.weight"];
    if (embeddingMatrixObj.is_null() || !embeddingMatrixObj.is_object()) {
        Logger() << "No model embedding matrix found" << std::endl;
        return false;
    }

    const bool ok = LoadBF16EmbeddingMatrix(bytes, embeddingMatrixObj);
    if (!ok) {
        Logger() << "Failed to load embedding matrix" << std::endl;
        return false;
    }
    return true;
}

std::vector<float> SafeTensor::EmbeddingToken(const int token) {
    return this->embeddingMatrix[token];
}
