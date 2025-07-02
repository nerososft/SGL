//
// Created by neo on 25-6-24.
//

#ifndef MODEL_CONFIG_H
#define MODEL_CONFIG_H
#include <string>
#include <vector>

struct ConfigProperties {
    std::vector<std::string> architectures;
    bool attention_bias;
    float attention_dropout;
    uint64_t bos_token_id;
    uint64_t eos_token_id;
    uint64_t head_dim;
    std::string hidden_act;
    uint64_t hidden_size;
    float initializer_range;
    uint64_t intermediate_size;
    uint64_t max_position_embeddings;
    uint64_t max_window_layers;
    std::string model_type;
    uint64_t num_attention_heads;
    uint64_t num_hidden_layers;
    uint64_t num_key_value_heads;
    float rms_norm_eps;
    // "rope_scaling": null,
    uint64_t rope_theta;
    // "sliding_window": null,
    bool tie_word_embeddings;
    std::string torch_dtype;
    std::string transformers_version;
    bool use_cache;
    bool use_sliding_window;
    uint64_t vocab_size;
};

class Config {
    ConfigProperties properties{};

public:
    Config() = default;

    ~Config() = default;

    bool LoadFromFile(const std::string &path);

    [[nodiscard]] uint64_t GetHiddenLayerNums() const;

    [[nodiscard]] uint64_t GetHeadDim() const;

    [[nodiscard]] uint64_t GetRoPETheta() const;

    [[nodiscard]] uint64_t GetHiddenSize() const;
};

#endif //MODEL_CONFIG_H
