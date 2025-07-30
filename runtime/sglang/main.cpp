//
// Created by neo on 25-7-25.
//
#include "compiler/ShaderCompiler.h"
#include <fstream>
#include <iostream>
#include <shaderc/shaderc.hpp>
#include <sstream>

int main(int argc, char *argv[]) {
  const std::string source = R"(#version 450
layout (local_size_x = 256, local_size_y = 1, local_size_z = 1) in;

  layout (std430, binding = 0) buffer InputStorageBuffer {
    float data[];
  } inputBuffer;

  layout (std430, binding = 1) buffer InputStorageBuffer2 {
    float data[];
  } inputBuffer2;

  layout (std430, binding = 2) buffer OutputStorageBuffer {
    float data[];
  } outputBuffer;

  layout (push_constant) uniform Params {
    uint width1;
    uint height1;
    uint width2;
    uint height2;
    // TODO: params
  } params;

  void main() {
    uvec2 coord = gl_GlobalInvocationID.xy;

    if (inputBuffer.data.length() != inputBuffer2.data.length()) {
      outputBuffer.data[0] = 0xDE;
      outputBuffer.data[1] = 0xAD;
      outputBuffer.data[2] = 0x1;// for vec
      outputBuffer.data[3] = inputBuffer.data.length();
      outputBuffer.data[4] = inputBuffer2.data.length();
      return;
    }
    if (coord.x >= inputBuffer.data.length()) {
      return;
    }
    float x1 = inputBuffer.data[coord.x];
    float x2 = inputBuffer2.data[coord.x];
    outputBuffer.data[coord.x] = x1 + x2;
  })";

  const std::vector<uint32_t> spirv = ShaderCompiler::Compile(source);
  std::ofstream outputFile("../../../core/sglang/out.comp.spv",
                           std::ios::binary);
  if (outputFile.is_open()) {
    outputFile.write(reinterpret_cast<const char *>(spirv.data()),
                     spirv.size() * sizeof(uint32_t));
    outputFile.close();
  }

  return 1;
}