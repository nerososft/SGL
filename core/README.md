## SGL
将Vulkan封装为计算图结构，隐藏细节，简化使用，可以用来做一些通用计算加速，图像处理，渲染，机器学习等任务。

## 基础框架使用
以两个100000000个浮点数的向量逐元素相加为例

### 初始化GPU与计算图
```c++
std::vector<const char *> extensions = {};
VkGPUContext gpuCtx(extensions);
gpuCtx.Init();
ComputeGraph computeGraph(gpuCtx);
SubComputeGraph computeSubGraph(gpuCtx);
computeSubGraph.Init();
computeGraph.AddSubGraph(computeSubGraph);
```

### 创建计算/渲染节点

```c++
std::vector<VkDescriptorSetLayoutBinding> descriptorSetLayoutBindings;
descriptorSetLayoutBindings.push_back(VkGPUHelper::BuildDescriptorSetLayoutBinding(0, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1, VK_SHADER_STAGE_COMPUTE_BIT));
descriptorSetLayoutBindings.push_back(VkGPUHelper::BuildDescriptorSetLayoutBinding(1, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1, VK_SHADER_STAGE_COMPUTE_BIT));
descriptorSetLayoutBindings.push_back(VkGPUHelper::BuildDescriptorSetLayoutBinding(2, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1, VK_SHADER_STAGE_COMPUTE_BIT));
                                                     
ComputePipelineNode addNode(gpuCtx, "Add", SHADER(add.comp.glsl.spv), 0, descriptorSetLayoutBindings, (params.bodyPointsNums + 255) / 256, 1, 1);
addNode->CreateComputeGraphNode();
```
编写`shader`
```glsl
#version 450
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

void main() {
    uvec2 coord = gl_GlobalInvocationID.xy;
    if (coord.x >= inputBuffer.data.length()) {
        return;
    }
    float x1 = inputBuffer.data[coord.x];
    float x2 = inputBuffer2.data[coord.x];
    outputBuffer.data[coord.x] = x1 + x2;
}
```

### 申请GPU内存并添加计算数据到计算节点

```c++
VkGPUBuffer inputBuffer(gpuCtx);
inputBuffer.AllocateAndBind(GPU_BUFFER_TYPE_STORAGE_SHARED, 100000000 * sizeof(float));
VkGPUBuffer inputBuffer2(gpuCtx);
inputBuffer2.AllocateAndBind(GPU_BUFFER_TYPE_STORAGE_SHARED, 100000000 * sizeof(float));
VkGPUBuffer outputBuffer(gpuCtx);
outputBuffer.AllocateAndBind(GPU_BUFFER_TYPE_STORAGE_SHARED, 100000000 * sizeof(float));
 
inputBuffer.UploadData(input1, 100000000 * sizeof(float));
inputBuffer2.UploadData(input.data(), 100000000 * sizeof(float));
 
std::vector<PipelineNodeBuffer> ppBuffers;
ppBuffers.push_back({ .type = PIPELINE_NODE_BUFFER_STORAGE_READ, .buf = { .bufferSize = inputBuffer->GetBufferSize(), .buffer = inputBuffer->GetBuffer()}});
ppBuffers.push_back({ .type = PIPELINE_NODE_BUFFER_STORAGE_READ, .buf = { .bufferSize = inputBuffer2->GetBufferSize(), .buffer = inputBuffer2->GetBuffer()}});
ppBuffers.push_back({ .type = PIPELINE_NODE_BUFFER_STORAGE_WRITE, .buf = { .bufferSize = outputBuffer->GetBufferSize(), .buffer = outputBuffer->GetBuffer()}});
addNode->AddComputeElement({ .pushConstantInfo = {}, .buffers = ppBuffers, .customDrawFunc = nullptr);
computeSubGraph->AddComputeGraphNode(bezierNode);
```

### 计算
```c++
computeGraph.Compute();
```

### 获取结果
```c++
outputBuffer.MapBuffer();
float *result = outputBuffer.GetMappedAddr();
```

## 图像处理算子使用
### 初始化滤镜框架
```c++
EffectEngine effectEngine;
effectEngine.Init();
```
### 使用滤镜
以高斯模糊为例
```c++
const auto filter = std::make_shared<FastGaussianBlurFilter>();
filter->SetRadius(120);
ImageInfo 
effectEngine.Process({.width = 1024, .height = 768, .channels = 4 .bytesPerLine = 4 * 1024, .data = inputPixelData}, {.width = 1024, .height = 768, .channels = 4 .bytesPerLine = 4 * 1024, .data = outputPixelData}, filter);
```

## 机器学习算子使用
