//
// Created by neo on 25-6-24.
//

#ifndef TOKENIZER_H
#define TOKENIZER_H
#include <memory>
#include <unordered_map>
#include <vector>

#include "SafeTensor.h"
#include "vendor/json.hpp"

class Tokenizer {
    int bosTokenId = -1;
    int eosTokenId = -1;
    std::unordered_map<std::string, int> vocab;
    std::vector<std::pair<std::string, std::string> > merges;
    std::unordered_map<std::string, std::string> bpeRanks;

public:
    Tokenizer() = default;

    ~Tokenizer() = default;

    bool LoadVocabularyFromJsonObject(nlohmann::json::const_reference array);

    bool LoadVocabularyFromJsonFile(nlohmann::json::const_reference str);

    bool LoadMergesFromJsonArray(nlohmann::json::const_reference array);

    bool LoadMergesFromJsonFile(nlohmann::json::const_reference str);

    bool LoadFromFile(const std::string &filePath);

    bool MergeToken(std::vector<std::string> &tokens);

    [[nodiscard]] std::vector<int> Encode(const std::string &text);

    [[nodiscard]] std::string DecodeOne(int prevToken, int token) const;
};


#endif //TOKENIZER_H
