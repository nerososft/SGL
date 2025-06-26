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

SafeTensor::SafeTensor(const std::shared_ptr<Config> &config) {
    this->config = config;

    this->weights.resize(config->GetHiddenLayerNums());
}

bool SafeTensor::LoadWeight(const std::string &weightName, nlohmann::json::const_reference weightData) {
    Logger(Logger::DEBUG) << "Loading weight " << weightName << "..." << std::endl;

    Weight weight;
    if (weightData["dtype"].is_string()) {
        if (weightData["dtype"] == "BF16") {
            weight.dtype = BF16;
        }
    }
    if (weightData["data_offsets"].is_array()) {
        weight.dataOffsets.start = weightData["data_offsets"][0];
        weight.dataOffsets.end = weightData["data_offsets"][1];
    }
    if (weightData["shape"].is_array()) {
        if (weightData["shape"].size() == 1) {
            weight.shape.width = weightData["shape"][0];
            weight.shape.height = 1;
        }
        if (weightData["shape"].size() == 2) {
            weight.shape.width = weightData["shape"][0];
            weight.shape.height = weightData["shape"][1];
        }
    }

    // TODO: Optmize this shit
    for (size_t i = 0; i < this->weights.size(); i++) {
        std::string prefix = "model.layers." + std::to_string(i) + ".";
        if (weightName.find(prefix) != std::string::npos) {
            this->weights[i].emplace(std::make_pair(weightName, weight));
        }
    }

    Logger(Logger::DEBUG) << "Loading weight " << weightName << " done" << std::endl;
    return true;
}

bool SafeTensor::LoadWeights(const std::vector<char> &safeTensorData,
                             nlohmann::json::const_reference header) {
    Logger(Logger::DEBUG) << "Weights loading..." << std::endl;
    for (auto it = header.begin(); it != header.end(); ++it) {
        if (!it.value().is_object()) {
            return false;
        }
        if (it.key() == "__metadata__") {
            continue;
        }
        if (!it.value().contains("dtype") ||
            !it.value().contains("shape") ||
            !it.value().contains("data_offsets")) {
            return false;
        }

        if (!LoadWeight(it.key(), it.value())) {
            return false;
        }
    }

    Logger(Logger::DEBUG) << "Weights loaded" << std::endl;
    return true;
}

bool SafeTensor::LoadBF16EmbeddingMatrix(const std::vector<char> &safeTensorData,
                                         nlohmann::json::const_reference header) {
    Logger(Logger::DEBUG) << "Embedding matrix loading..." << std::endl;
    const nlohmann::json embeddingMatrixObj = header["model.embed_tokens.weight"];
    if (embeddingMatrixObj.is_null() || !embeddingMatrixObj.is_object()) {
        Logger(Logger::DEBUG) << "No model embedding matrix found" << std::endl;
        return false;
    }

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

    Logger(Logger::DEBUG) << "Embedding matrix range: (" << dataBegin << ", " << dataEnd << ")" << std::endl;
    Logger(Logger::DEBUG) << "Embedding matrix shape: (" << shapeHeight << ", " << shapeWidth << ")" << std::endl;
    Logger(Logger::DEBUG) << "Embedding matrix data size: " << dataSize << std::endl;

    const uint16_t *pf16Data = (uint16_t *) (safeTensorData.data() + dataBegin);
    this->embeddingMatrix.resize(shapeHeight);
    for (uint64_t i = 0; i < shapeHeight; i++) {
        this->embeddingMatrix[i].resize(shapeWidth);
        for (uint64_t j = 0; j < shapeWidth; j++) {
            this->embeddingMatrix[i][j] = BF16ToFP32(pf16Data[i * shapeWidth + j]);
        }
    }
    Logger(Logger::DEBUG) << "Embedding matrix loaded" << std::endl;
    return true;
}

bool SafeTensor::LoadFromFile(const std::string &tensorFilePath) {
    const std::vector<char> bytes = IOUtils::ReadFile(tensorFilePath); // TODO: mmap should be better for large file
    if (bytes.empty()) {
        Logger(Logger::DEBUG) << "Failed '" << tensorFilePath << "' is empty" << std::endl;
    }
    const char *data = bytes.data();
    if (data == nullptr) {
        Logger(Logger::DEBUG) << "Failed '" << tensorFilePath << "' is empty" << std::endl;
        return false;
    }
    const uint64_t headSize = ((uint64_t *) (data))[0];
    Logger(Logger::DEBUG) << "Head size: " << headSize << std::endl;

    auto headerJsonStr = std::string(data + 8, headSize);
    const nlohmann::json header = nlohmann::json::parse(headerJsonStr);

    bool ok = LoadBF16EmbeddingMatrix(bytes, header);
    if (!ok) {
        Logger(Logger::DEBUG) << "Failed to load embedding matrix" << std::endl;
        return false;
    }

    ok = LoadWeights(bytes, header);
    if (!ok) {
        Logger(Logger::DEBUG) << "Failed to load weights" << std::endl;
        return false;
    }

    return true;
}

std::vector<float> SafeTensor::EmbeddingToken(const int token) {
    return this->embeddingMatrix[token];
}

Weight SafeTensor::GetLayerWeight(const size_t layerIndex,
                                  const std::string &name) {
    if (layerIndex >= this->weights.size()) {
        Logger(Logger::ERROR) << "Invalid layer index" << std::endl;
        exit(-1);
    }

    const std::string layerName = "model.layers." + std::to_string(layerIndex) + "." + name + ".weight";
    Logger(Logger::DEBUG) << "Loading layer weight: " << layerName << std::endl;

    const auto it = this->weights[layerIndex].find(layerName);
    if (it == this->weights[layerIndex].end()) {
        Logger(Logger::ERROR) << "Not found '" << layerName << "' from weights" << std::endl;
        exit(-1);
    }

    return it->second;
}
