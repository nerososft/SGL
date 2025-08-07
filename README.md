
<table>
<tr>

<td width="20%">
<img src="https://raw.gitcode.com/OpenHarmonyPerfSquad/SGL/blobs/9022a326d73aa21fcbd1a08e9458fa5b5ec32a89/logo_t.png">
</td>

<td>

<h1>SGL(Simple GPU Layer)</h1>
<h3>High-performance GPU compute framework based on Vulkan</h3>

</td>

</tr>
</table>

[![Ask DeepWiki](https://deepwiki.com/badge.svg)](https://deepwiki.com/nerososft/SGL)
[Documentation](https://gitcode.com/OpenHarmonyPerfSquad/SGL/blob/main/README.md) [Examples](https://gitcode.com/OpenHarmonyPerfSquad/SGL/tree/main/examples) 

```c++
auto mat1 = ce->CreateMatrix(width, height, data1);
auto mat2 = ce->CreateMatrix(width, height, data2);
auto matOutput = ce->CreateMatrix(width, height);
ce->Seq()->Record(ce->MatMul(mat1, mat2, matOutput))->Eval()->Destroy();
```

---

## Base Framework usage

Taking the element-wise addition of two floating-point vectors `float input1[100000000]` and `float input2[100000000]` as an example

### 1. initialize device and compute graph

```c++
std::vector<const char *> extensions = {};
VkGPUContext gpuCtx(extensions);
gpuCtx.Init();
ComputeGraph computeGraph(gpuCtx);
SubComputeGraph computeSubGraph(gpuCtx);
computeSubGraph.Init();
computeGraph.AddSubGraph(computeSubGraph);
```

### 2. Create compute/renderer node

```c++
std::vector<VkDescriptorSetLayoutBinding> descriptorSetLayoutBindings;
descriptorSetLayoutBindings.push_back(VkGPUHelper::BuildDescriptorSetLayoutBinding(0, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1, VK_SHADER_STAGE_COMPUTE_BIT));
descriptorSetLayoutBindings.push_back(VkGPUHelper::BuildDescriptorSetLayoutBinding(1, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1, VK_SHADER_STAGE_COMPUTE_BIT));
descriptorSetLayoutBindings.push_back(VkGPUHelper::BuildDescriptorSetLayoutBinding(2, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1, VK_SHADER_STAGE_COMPUTE_BIT));
                                                     
ComputePipelineNode addNode(gpuCtx, "Add", SHADER(add.comp.glsl.spv), 0, descriptorSetLayoutBindings, (100000000 + 255) / 256, 1, 1);
addNode->CreateComputeGraphNode();
```

write `shader`

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

### 3. GPU buffer allocation and data upload download

```c++
VkGPUBuffer inputBuffer(gpuCtx);
inputBuffer.AllocateAndBind(GPU_BUFFER_TYPE_STORAGE_SHARED, 100000000 * sizeof(float));
VkGPUBuffer inputBuffer2(gpuCtx);
inputBuffer2.AllocateAndBind(GPU_BUFFER_TYPE_STORAGE_SHARED, 100000000 * sizeof(float));
VkGPUBuffer outputBuffer(gpuCtx);
outputBuffer.AllocateAndBind(GPU_BUFFER_TYPE_STORAGE_SHARED, 100000000 * sizeof(float));
 
inputBuffer.UploadData(input1, 100000000 * sizeof(float));
inputBuffer2.UploadData(input2, 100000000 * sizeof(float));
 
std::vector<PipelineNodeBuffer> ppBuffers;
ppBuffers.push_back({ .type = PIPELINE_NODE_BUFFER_STORAGE_READ, .buf = { .bufferSize = inputBuffer->GetBufferSize(), .buffer = inputBuffer->GetBuffer()}});
ppBuffers.push_back({ .type = PIPELINE_NODE_BUFFER_STORAGE_READ, .buf = { .bufferSize = inputBuffer2->GetBufferSize(), .buffer = inputBuffer2->GetBuffer()}});
ppBuffers.push_back({ .type = PIPELINE_NODE_BUFFER_STORAGE_WRITE, .buf = { .bufferSize = outputBuffer->GetBufferSize(), .buffer = outputBuffer->GetBuffer()}});
addNode->AddComputeElement({ .pushConstantInfo = {}, .buffers = ppBuffers, .customDrawFunc = nullptr);
computeSubGraph->AddComputeGraphNode(bezierNode);
```

### 4. compute

```c++
computeGraph.Compute();
```

### 5. result

```c++
outputBuffer.MapBuffer();
float *result = outputBuffer.GetMappedAddr();
```

---

## Image processing usage

### 1. initialize image engine

```c++
ImageEngine imageEngine;
imageEngine.Init();
```

### 2. use builtin filters

e.g.

```c++
const auto filter = std::make_shared<FastGaussianBlurFilter>();
filter->SetRadius(120);
imageEngine.Process({.width = 1024, .height = 768, .channels = 4 .bytesPerLine = 4 * 1024, .data = inputPixelData}, {.width = 1024, .height = 768, .channels = 4 .bytesPerLine = 4 * 1024, .data = outputPixelData}, filter);
```

builtin filters:

- [chinese_paint]()
- [color_pencil_sketch]()
- [color_separation]()
- [crystallize]()
- [custom_kernel]()
- [filter.templete]()
- [focus_blur]()
- [gray]()
- [horizontal_blur_old]()
- [horizontal_blur]()
- [midvalue]()
- [nes8bit]()
- [palette_knife]()
- [palette_q]()
- [pathblur]()
- [radial_blur]()
- [scale]()
- [surface_blur]()
- [templete]()
- [threshold_split]()
- [transform]()
- [transform3d.frag]()
- [transform3d.vert]()
- [vertical_blur_old]()
- [vertical_blur]()
- [vibrance]()
- [voronoi]()

### 3. custom filter

Refer to `Basic Framework Usage` for implementation.

---

## Compute engine usage

### 1. Initialize
```c++
auto ce = std::make_shared<ComputeEngine>();
ce->Init()
```
### 2. Initialize matrix
```c++
auto mat = ce->CreateMatrix(width, height);
```
### 3. Use builtin operators
```c++
 const std::shared_ptr<Sequence> seq = ce->Seq()
            ->Record(ce->LayerNorm(inputsMatrix[tokenPos],
                                    this->inputLayerNorm,
                                    biasMatrix,
                                    1e-06,
                                    true,
                                    false,
                                    inputLayerNormOutput[tokenPos]))
            ->Record(ce->MatMul(inputLayerNormOutput[tokenPos], selfAttnQProj, qProjOutput[tokenPos]))
            ->Record(ce->MatMul(inputLayerNormOutput[tokenPos], selfAttnKProj, kProjOutput[tokenPos]))
            ->Record(ce->MatMul(inputLayerNormOutput[tokenPos], selfAttnVProj, vProjOutput[tokenPos]))
            ->Record(ce->Split(qProjOutput[tokenPos], queryHeadNums, qHeads[tokenPos]))
            ->Eval()
            ->Destroy();
```

builtin operators：

- [add]()
- [concat8]()
- [concat16]()
- [gelu]()
- [layer_norm]()
- [matmul]()
- [mul]()
- [relu]()
- [rms_norm]()
- [rope]()
- [scale_d]()
- [sigmoid]()
- [silu]()
- [softmax]()
- [split8]()
- [split16]()
- [tanh]()
- [sin]()
- [cos]()
- [exp]()
- [log]()
### 4. custom operator

Refer to `Basic Framework Usage` for implementation.

## render and visualization

### 1. setup callback of scene load
```c++
const auto renderer = std::make_shared<Renderer>(768, 768);
renderer->SetOnLoadScene([](Renderer *rdr) -> bool {
        const std::vector<std::shared_ptr<Mesh> > models = ModelLoader::LoadModel("./DamagedHelmet.gltf");
        for (auto &mesh: models) {
            rdr->AddDrawElement(mesh->vertexData, mesh->indicesData, mesh->material, mesh->transform);
        }
        return true;
    }
);
```

### 2. setup callback of camera init
initialize camera
```c++
renderer->SetOnRendererReady([](Renderer *rdr) -> bool {
    const std::shared_ptr<RendererCamera> camera = rdr->GetCamera();
    const glm::mat4 view = camera->GetViewMatrix();
    camera->SetViewMatrix(glm::rotate(view, glm::radians(45.0f), glm::vec3(0.0f, 1.0f, 0.0f)));
    return true;
});
```

### 3. initialize renderer
```c++
renderer->Init();
```

### 4. render offscreen
```c++
renderer->RenderFrameOffScreen("./render_offscreen1.png");
```