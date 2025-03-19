# EffectEngine(基于GPU计算管线的高性能离线图像处理引擎)
1. **性能优越（GPU并行）**
   1. 相对于CPU OpenMP等技术，并行能力更强
   2. 释放异构算力
2. **突破渲染管线贴图大小限制**
   1. 是用StorageBuffer， 相比Image/ImageView/Sampler，没有大小限制
3. **新增滤镜简单灵活（ComputeGraph设计）**
   1. ComputeGraph 设计，更加通用灵活
4. **引擎与滤镜算法实现解耦（加载预编译的shader运行）**
   1. 可实现滤镜在线下载
5. **更好的兼容性（相比于OpenCL,CUDA）**
   1. 基本上GPU都支持vulkan