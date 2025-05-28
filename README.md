# SpiristoneEngine

基于Vulkan的高性能GPU计算引擎 (High-Performance GPU Compute Engine Based on Vulkan)

## 特性 (Features)
1. **计算管线/图形管线均支持**
2. **灵活的ComputeGraph架构**
3. **模块化设计**
4. **优秀的兼容性**

## Env
### 图像效果引擎
`effect_engine` 依赖：`vulkan`

### 机器学习引擎
`ml_engine` 依赖 `vulkan`

### 渲染引擎
`render_engine` 依赖：`vulkan` `libglm-dev`

### demo
`effect_demo` 依赖 `effect_engine` `libpng-dev`
`renderer_demo` 依赖：`render_engine` `libpng-dev` `libglfw3-dev` `assimp`
`inference_demo` 依赖 `ml_engine`