//
// Created by neo on 25-7-25.
//

#ifndef SHADERCOMPILER_H
#define SHADERCOMPILER_H
#include <vector>

class ShaderCompiler {
public:
  ShaderCompiler() = default;
  ~ShaderCompiler() = default;
  static std::vector<uint32_t> Compile(const std::string &source);
};

#endif // SHADERCOMPILER_H
