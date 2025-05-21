# EffectEngine

基于Vulkan的高性能图像引擎 (High-Performance Graphics Engine Based on Vulkan)

## 特性 (Features)
1. **计算管线/图形管线均支持**
2. **灵活的ComputeGraph架构**
3. **模块化设计**
4. **优秀的兼容性**

## Env
`effect_engine` 依赖：`vulkan`

`render_engine` 依赖：`vulkan` `libglm-dev`

`effect_demo` 依赖 `effect_engine` `libpng-dev`

`renderer_demo` 依赖：`render_engine` `libpng-dev` `libglfw3-dev` `assimp`