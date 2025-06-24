//
// Created by neo on 25-6-24.
//

#include "Tokenizer.h"

#include <fstream>
#include <iostream>

#include "core/log/Log.h"
#include "core/utils/IOUtils.h"
#include "vendor/json.hpp"

bool Tokenizer::LoadVocabularyFromJsonObject(nlohmann::json::const_reference array) {
    if (!array.is_object()) {
        Logger() << "Not an object" << std::endl;
        return false;
    }
    std::cout << "Loading vocabulary from json object, size: " << array.size() << std::endl;
    for (auto &item: array.items()) {
        this->vocab[item.key()] = item.value();
    }
    return true;
}

bool Tokenizer::LoadVocabularyFromJsonFile(nlohmann::json::const_reference str) {
    // TODO:
    return false;
}

bool Tokenizer::LoadMergesFromJsonArray(nlohmann::json::const_reference array) {
    if (!array.is_array()) {
        Logger() << "Not an array" << std::endl;
        return false;
    }
    std::cout << "Loading merge from json array, size: " << array.size() << std::endl;
    this->merges.resize(array.size());
    for (auto &pair: array) {
        this->merges.emplace_back(std::make_pair(pair[0], pair[1]));
    }
    for (size_t i = 0; i < merges.size(); ++i) {
        const auto &[part1, part2] = merges[i];
        bpeRanks[part1 + " " + part2] = std::to_string(i);
    }
    return true;
}

bool Tokenizer::LoadMergesFromJsonFile(nlohmann::json::const_reference str) {
    // TODO:
    return false;
}

bool Tokenizer::LoadFromFile(const std::string &filePath) {
    std::ifstream f(filePath);
    if (f.fail()) {
        Logger() << "Failed to open file " << filePath << std::endl;
        return false;
    }
    const nlohmann::json data = nlohmann::json::parse(f);
    if (data.is_null()) {
        Logger() << "Failed to parse JSON file " << filePath << std::endl;
        return false;
    }

    Logger() << "Loading Tokenizer from file: " << filePath << std::endl;
    if (!data.is_object()) {
        Logger() << "Not a valid tokenizer file " << filePath << std::endl;
        return false;
    }

    if (data["model"].is_null() || !data["model"].is_object()) {
        Logger() << "No model found, not a valid tokenizer file " << filePath << std::endl;
        return false;
    }

    if (data["model"]["vocab"].is_object()) {
        LoadVocabularyFromJsonObject(data["model"]["vocab"]);
    } else if (data["model"]["vocab"].is_string()) {
        LoadVocabularyFromJsonFile(data["model"]["vocab"]);
    }

    if (data["model"]["merges"].is_array()) {
        LoadMergesFromJsonArray(data["model"]["merges"]);
    } else if (data["model"]["merges"].is_string()) {
        LoadMergesFromJsonFile(data["model"]["merges"]);
    }
    Logger() << "Loaded Tokenizer" << std::endl;
    return true;
}

bool Tokenizer::MergeToken(std::vector<std::string> &tokens) {
    if (tokens.size() < 2) {
        return false;
    }
    std::string bestPair;
    int bestRank = -1;
    size_t bestIdx = 0;

    for (size_t i = 0; i < tokens.size() - 1; ++i) {
        std::string pair = tokens[i] + " " + tokens[i + 1];
        auto it = this->bpeRanks.find(pair);
        if (it != this->bpeRanks.end()) {
            const int rank = std::stoi(it->second);
            if (bestRank == -1 || rank < bestRank) {
                bestRank = rank;
                bestPair = tokens[i] + tokens[i + 1];
                bestIdx = i;
            }
        }
    }

    if (bestRank != -1) {
        tokens[bestIdx] = bestPair;
        tokens.erase(tokens.begin() + bestIdx + 1);
        return true;
    }
    return false;
}

std::vector<int> Tokenizer::Encode(const std::string &text) {
    Logger() << "Encode: '" << text << "'" << std::endl;
    std::vector<std::string> tokens;
    for (const auto &c: text) {
        tokens.push_back(std::string(1, c));
    }

    while (MergeToken(tokens)) {}

    std::vector<int> result;
    for (const auto &token: tokens) {
        auto it = this->vocab.find(token);
        if (it != this->vocab.end()) {
            result.push_back(it->second);
        }
    }
    return result;
}

std::string Tokenizer::DecodeOne(int prevToken, int token) const {
}
