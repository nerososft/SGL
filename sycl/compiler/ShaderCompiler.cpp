//
// Created by neo on 25-7-25.
//

#include "ShaderCompiler.h"

#include "runtime/log/Log.h"

#include <shaderc/shaderc.hpp>

std::vector<uint32_t> ShaderCompiler::Compile(const std::string &source) {
  const shaderc::Compiler compiler;
  shaderc::CompileOptions options;
  options.SetOptimizationLevel(shaderc_optimization_level_performance);

  const auto result = compiler.CompileGlslToSpv(
      source, shaderc_glsl_compute_shader, "add", options);
  if (result.GetCompilationStatus() != shaderc_compilation_status_success) {
    Logger() << Logger::ERROR
             << "Shader compilation error:" << result.GetErrorMessage()
             << std::endl;
    return {};
  }

  return {result.begin(), result.end()};
}