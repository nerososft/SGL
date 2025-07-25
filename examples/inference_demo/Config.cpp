//
// Created by neo on 25-6-24.
//

#include "Config.h"

#include "runtime/log/Log.h"
#include "vendor/json.hpp"

bool Config::LoadFromFile(const std::string &configFilePath) {
  Logger() << "LoadConfigFromFile: '" << configFilePath << std::endl;
  std::ifstream f(configFilePath);
  if (f.fail()) {
    Logger() << "Failed to open file " << configFilePath << std::endl;
    return false;
  }
  nlohmann::json::const_reference config = nlohmann::json::parse(f);
  for (auto it = config.begin(); it != config.end(); ++it) {
    Logger(Logger::DEBUG) << it.key() << ": " << it.value() << std::endl;
  }

  this->properties.num_hidden_layers = config["num_hidden_layers"];
  this->properties.head_dim = config["head_dim"];
  this->properties.rope_theta = config["rope_theta"];
  this->properties.hidden_size = config["hidden_size"];
  return true;
}

uint64_t Config::GetHiddenLayerNums() const {
  return this->properties.num_hidden_layers;
}

uint64_t Config::GetHeadDim() const { return this->properties.head_dim; }

uint64_t Config::GetRoPETheta() const { return this->properties.rope_theta; }

uint64_t Config::GetHiddenSize() const { return this->properties.hidden_size; }
