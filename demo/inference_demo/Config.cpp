//
// Created by neo on 25-6-24.
//

#include "Config.h"

#include "core/log/Log.h"
#include "vendor/json.hpp"

bool Config::LoadFromFile(const std::string &configFilePath) {
    Logger() << "LoadModelFromFile: '" << configFilePath << std::endl;
    std::ifstream f(configFilePath);
    nlohmann::json::const_reference config = nlohmann::json::parse(f);
    for (auto it = config.begin(); it != config.end(); ++it) {
        std::cout << it.key() << ": " << it.value() << std::endl;
    }
}
