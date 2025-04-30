# EffectEngine

基于Vulkan的高性能图像引擎 (High-Performance Graphics Engine Based on Vulkan)

## 特性 (Features)
1. **计算管线/图形管线均支持**
   - 采用StorageBuffer技术，突破Image/ImageView/Sampler的贴图大小限制
   - 支持超大分辨率图像处理
   - 灵活的内存管理机制

2. **灵活的ComputeGraph架构**
   - 基于ComputeGraph的通用设计
   - 支持快速开发和集成新的图像处理算法
   - 简单直观的API接口

3. **模块化设计**
   - 引擎核心与滤镜算法完全解耦
   - 支持预编译shader的动态加载
   - 可支持滤镜的在线下载

4. **优秀的兼容性**
   - 基于Vulkan实现，支持主流GPU平台
   - 相比OpenCL和CUDA具有更广泛的硬件支持
   - 跨平台支持

## Env
`effect_engine`依赖：`vulkan`

`render_engine`依赖：`vulkan` `libglm-dev`

`effect_demo` 依赖 `effect_engine` `libpng-dev`

`render_demo` 依赖：`render_engine` `libpng-dev` `libglfw3-dev` `assimp`