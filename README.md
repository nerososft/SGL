# EffectEngine

基于GPU计算管线的高性能离线图像处理引擎 (High-Performance Offline Image Processing Engine Based on GPU Compute Pipeline)

## 特性 (Features)

1. **高性能GPU并行计算**
   - 相比CPU OpenMP等技术，具有更强的并行处理能力
   - 充分利用异构计算资源，显著提升处理效率
   - 支持大规模图像数据的快速处理

2. **突破传统渲染管线限制**
   - 采用StorageBuffer技术，突破Image/ImageView/Sampler的贴图大小限制
   - 支持超大分辨率图像处理
   - 灵活的内存管理机制

3. **灵活的ComputeGraph架构**
   - 基于ComputeGraph的通用设计
   - 支持快速开发和集成新的图像处理算法
   - 简单直观的API接口

4. **模块化设计**
   - 引擎核心与滤镜算法完全解耦
   - 支持预编译shader的动态加载
   - 可支持滤镜的在线下载

5. **优秀的兼容性**
   - 基于Vulkan实现，支持主流GPU平台
   - 相比OpenCL和CUDA具有更广泛的硬件支持
   - 跨平台支持

## 快速开始 (Quick Start)

### 环境要求
- Vulkan SDK
- CMake 3.0+
- C++17兼容的编译器

## 项目结构 (Project Structure)

```
effect_engine/
├── assets/          # 资源文件
├── demo/            # 示例代码
├── effect_engine/   # 引擎核心代码
└── shader_compiled/ # 预编译的shader文件
```

## 贡献指南 (Contributing)

欢迎提交Issue和Pull Request。