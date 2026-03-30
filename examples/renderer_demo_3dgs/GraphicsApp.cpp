//
// Created by neo on 2025/2/9.
//

#include "GraphicsApp.h"

#include "3dgs/SpzModel.h"
#include "runtime/gpu/VkGPUHelper.h"
#include "runtime/log/Log.h"
#include "window/impl/GLFWWindowImpl.h"

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <cfloat>
#include <glm/ext/matrix_transform.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/vec3.hpp>
#include <numeric>

VkSurfaceKHR GetWindowSurface(const VkInstance instance) {
  const SurfaceGetParams params{.params = {
                                    .vkInstance = instance,
                                }};
  return window->GetSurface(params).surface.vkSurface;
}

bool GraphicsApp::ConstructRendererPipeline() {
  std::vector<VkVertexInputBindingDescription> vertexInputBindingDescriptions =
      {{.binding = 0,
        .stride = sizeof(GaussianSplatVertex),
        .inputRate = VK_VERTEX_INPUT_RATE_VERTEX}};

  std::vector<VkVertexInputAttributeDescription>
      vertexInputAttributeDescriptions = {};
  vertexInputAttributeDescriptions.push_back(
      {.location = 0, // position
       .binding = 0,
       .format = VK_FORMAT_R32G32B32A32_SFLOAT,
       .offset = offsetof(GaussianSplatVertex, position)});
  vertexInputAttributeDescriptions.push_back(
      {.location = 1, // color
       .binding = 0,
       .format = VK_FORMAT_R32G32B32A32_SFLOAT,
       .offset = offsetof(GaussianSplatVertex, color)});
  vertexInputAttributeDescriptions.push_back(
      {.location = 2, // scale
       .binding = 0,
       .format = VK_FORMAT_R32G32B32A32_SFLOAT,
       .offset = offsetof(GaussianSplatVertex, scale)});
  vertexInputAttributeDescriptions.push_back(
      {.location = 3, // rotate
       .binding = 0,
       .format = VK_FORMAT_R32G32B32A32_SFLOAT,
       .offset = offsetof(GaussianSplatVertex, rotate)});
  vertexInputAttributeDescriptions.push_back(
      {.location = 4, // opacity
       .binding = 0,
       .format = VK_FORMAT_R32G32B32A32_SFLOAT,
       .offset = offsetof(GaussianSplatVertex, opacity)});
  vertexInputAttributeDescriptions.push_back(
      {.location = 5, // quadCoord
       .binding = 0,
       .format = VK_FORMAT_R32G32_SFLOAT,
       .offset = offsetof(GaussianSplatVertex, quadCoord)});

  std::vector<VkDescriptorSetLayoutBinding> descriptorSetLayoutBindings;
  descriptorSetLayoutBindings.push_back(
      VkGPUHelper::BuildDescriptorSetLayoutBinding(
          0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1,
          VK_SHADER_STAGE_ALL_GRAPHICS));

  VkViewport viewport{
      .x = 0,
      .y = 0,
      .width = static_cast<float>(windowWidth),
      .height = static_cast<float>(windowHeight),
      .minDepth = 0,
      .maxDepth = 1,
  };
  this->graphicsPipelineNode = std::make_shared<GraphicsPipelineNode>(
      renderer->GetGPUCtx(), "3DGSGraphicsPipeline",
      this->renderer->GetMainRenderPass(), SHADER(rect_3dgs.vert.glsl.spv),
      SHADER(rect_3dgs.frag.glsl.spv), sizeof(DemoFrameInfo),
      descriptorSetLayoutBindings, vertexInputBindingDescriptions,
      vertexInputAttributeDescriptions, this->windowWidth, this->windowHeight,
      viewport, VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, VK_POLYGON_MODE_FILL, true,
      false);
  if (this->graphicsPipelineNode == nullptr) {
    Logger() << Logger::ERROR << "Failed to create graphics pipeline node!"
             << std::endl;
    return false;
  }

  const VkResult ret = this->graphicsPipelineNode->CreateComputeGraphNode();
  if (ret != VK_SUCCESS) {
    Logger() << Logger::ERROR << "Failed to create graphics pipeline node!"
             << std::endl;
    return false;
  }

  renderer->AddRenderGraph(this->graphicsPipelineNode);

  return true;
}

void GraphicsApp::UpdateSceneState(const float elapsedSeconds) {
  this->frameInfo.time = elapsedSeconds;

  const float orbitRadius = 2.8f;
  const float orbitHeight = 0.35f;
  const float orbitSpeed = 0.35f;
  const float angle = elapsedSeconds * orbitSpeed;
  const glm::vec3 eye(std::sin(angle) * orbitRadius, orbitHeight,
                      std::cos(angle) * orbitRadius);

  this->cameraUniform.view = glm::lookAt(eye, glm::vec3(0.0f),
                                         glm::vec3(0.0f, 1.0f, 0.0f));
  this->cameraUniform.proj = glm::perspective(
      glm::radians(50.0f),
      static_cast<float>(this->windowWidth) /
          static_cast<float>(std::max<uint32_t>(this->windowHeight, 1)),
      0.01f, 10.0f);
  this->cameraUniform.proj[1][1] *= -1.0f;
  this->cameraUniform.camPos = glm::vec4(eye, 1.0f);
  // x = gaussian extent in sigma, y = covariance epsilon, z = opacity floor,
  // w = max NDC axis length clamp
  this->cameraUniform.renderParams =
      glm::vec4(3.0f, 1e-6f, 0.02f, 0.18f);
  this->UpdateSortedIndices();

  if (this->cameraBuffer != nullptr) {
    const VkResult ret = this->cameraBuffer->UploadData(&this->cameraUniform,
                                                        sizeof(this->cameraUniform));
    if (ret != VK_SUCCESS) {
      Logger() << Logger::ERROR << "Failed to update camera buffer"
               << std::endl;
    }
  }
}

void GraphicsApp::UpdateSortedIndices() {
  if (this->indexBuffer == nullptr || this->gaussianPoints.empty()) {
    return;
  }

  if (this->sortedPointOrder.size() != this->gaussianPoints.size()) {
    this->sortedPointOrder.resize(this->gaussianPoints.size());
    std::iota(this->sortedPointOrder.begin(), this->sortedPointOrder.end(), 0);
  }
  if (this->sortDepths.size() != this->gaussianPoints.size()) {
    this->sortDepths.resize(this->gaussianPoints.size());
  }

  for (size_t i = 0; i < this->gaussianPoints.size(); ++i) {
    const glm::vec4 viewPos = this->cameraUniform.view *
                              this->gaussianPoints[i].position;
    this->sortDepths[i] = viewPos.z;
  }

  std::stable_sort(this->sortedPointOrder.begin(), this->sortedPointOrder.end(),
                   [this](const uint32_t lhs, const uint32_t rhs) {
                     return this->sortDepths[lhs] < this->sortDepths[rhs];
                   });

  if (this->sortedIndices.size() != this->gaussianPoints.size() * 6) {
    this->sortedIndices.resize(this->gaussianPoints.size() * 6);
  }
  static constexpr uint32_t kQuadIndexPattern[6] = {0, 1, 2, 2, 1, 3};
  for (size_t sortedIdx = 0; sortedIdx < this->sortedPointOrder.size();
       ++sortedIdx) {
    const uint32_t pointIndex = this->sortedPointOrder[sortedIdx];
    const uint32_t baseVertex = pointIndex * 4;
    uint32_t *dst = this->sortedIndices.data() + sortedIdx * 6;
    for (uint32_t idx = 0; idx < 6; ++idx) {
      dst[idx] = baseVertex + kQuadIndexPattern[idx];
    }
  }

  const VkResult ret = this->indexBuffer->UploadData(
      this->sortedIndices.data(),
      this->sortedIndices.size() * sizeof(this->sortedIndices[0]));
  if (ret != VK_SUCCESS) {
    Logger() << Logger::ERROR << "Failed to upload sorted index buffer"
             << std::endl;
  }
}

bool GraphicsApp::Init() {
  window = std::make_shared<GLFWWindowImpl>(this);
  const std::vector<const char *> requiredExtensions =
      window->GetRendererRequiredExtensions();
  renderer = std::make_shared<Renderer>(this->windowWidth, this->windowHeight);

  window->CreateWindow(this->windowPosX, this->windowPosY, this->windowWidth,
                       this->windowHeight, this->windowTitle, this->windowMode);
  if (!renderer->Init(requiredExtensions, GetWindowSurface)) {
    Logger() << Logger::ERROR << "Failed to initialize renderer" << std::endl;
    return false;
  }

  if (!this->ConstructRendererPipeline()) {
    Logger() << Logger::ERROR << "Failed to construct renderer" << std::endl;
    return false;
  }

  SpzModel model;
  if (!model.loadModel("../../../examples/renderer_demo_3dgs/assets/"
                       "builtin.models/3dgs/butterfly")) {
    Logger() << Logger::ERROR << "Failed to load model" << std::endl;
    return false;
  }

  std::vector<GaussianPoint> points = model.getPoints();
  if (points.empty()) {
    Logger() << Logger::ERROR << "Model has no points" << std::endl;
    return false;
  }

  glm::vec3 minBounds(MAXFLOAT);
  glm::vec3 maxBounds(-MAXFLOAT);
  for (auto &point : points) {
    const glm::vec3 pos(point.position);
    minBounds = glm::min(minBounds, pos);
    maxBounds = glm::max(maxBounds, pos);
  }

  const glm::vec3 center = (minBounds + maxBounds) * 0.5f;
  const glm::vec3 extent = maxBounds - minBounds;
  const float maxExtent =
      std::max(std::max(extent.x, extent.y), std::max(extent.z, 1e-5f));
  const float modelScale = 1.8f / maxExtent;
  for (auto &point : points) {
    point.position =
        glm::vec4((glm::vec3(point.position) - center) * modelScale, 1.0f);
    point.scale *= modelScale;
  }
  this->gaussianPoints = points;
  std::vector<GaussianSplatVertex> splatVertices;
  splatVertices.reserve(points.size() * 4);
  static constexpr glm::vec2 kQuadCoords[4] = {
      {-1.0f, -1.0f},
      {1.0f, -1.0f},
      {-1.0f, 1.0f},
      {1.0f, 1.0f},
  };
  for (const auto &point : points) {
    for (const auto quadCoord : kQuadCoords) {
      GaussianSplatVertex vertex{};
      vertex.position = point.position;
      vertex.color = point.color;
      vertex.scale = point.scale;
      vertex.rotate = point.rotate;
      vertex.opacity = point.opacity;
      vertex.quadCoord = quadCoord;
      splatVertices.push_back(vertex);
    }
  }
  this->vertexBuffer = std::make_shared<VkGPUBuffer>(renderer->GetGPUCtx());
  VkResult ret = vertexBuffer->AllocateAndBind(
      GPU_BUFFER_TYPE_VERTEX,
      splatVertices.size() * sizeof(GaussianSplatVertex));
  if (ret != VK_SUCCESS) {
    Logger() << Logger::ERROR << "Failed to allocate GPU buffer" << std::endl;
    return false;
  }
  ret = vertexBuffer->UploadData(
      splatVertices.data(),
      splatVertices.size() * sizeof(GaussianSplatVertex));
  if (ret != VK_SUCCESS) {
    Logger() << Logger::ERROR << "Failed to upload GPU buffer" << std::endl;
    return false;
  }

  PipelineNodeBuffer vertexBufferNode;
  vertexBufferNode.type = PIPELINE_NODE_BUFFER_VERTEX;
  vertexBufferNode.buf.buffer = vertexBuffer->GetBuffer();
  vertexBufferNode.buf.bufferSize = vertexBuffer->GetBufferSize();

  std::vector<uint32_t> indices(points.size() * 6, 0);
  this->indexBuffer = std::make_shared<VkGPUBuffer>(renderer->GetGPUCtx());
  ret = indexBuffer->AllocateAndBind(GPU_BUFFER_TYPE_INDEX,
                                     indices.size() * sizeof(uint32_t));
  if (ret != VK_SUCCESS) {
    Logger() << Logger::ERROR << "Failed to allocate GPU buffer" << std::endl;
    return false;
  }
  this->sortedPointOrder.resize(points.size());
  std::iota(this->sortedPointOrder.begin(), this->sortedPointOrder.end(), 0);
  static constexpr uint32_t kQuadIndexPattern[6] = {0, 1, 2, 2, 1, 3};
  for (uint32_t pointIdx = 0; pointIdx < points.size(); ++pointIdx) {
    const uint32_t baseVertex = pointIdx * 4;
    uint32_t *dst = indices.data() + pointIdx * 6;
    for (uint32_t idx = 0; idx < 6; ++idx) {
      dst[idx] = baseVertex + kQuadIndexPattern[idx];
    }
  }
  this->sortedIndices = indices;
  this->sortDepths.assign(points.size(), 0.0f);
  ret = indexBuffer->UploadData(indices.data(),
                                indices.size() * sizeof(uint32_t));
  if (ret != VK_SUCCESS) {
    Logger() << Logger::ERROR << "Failed to upload GPU buffer" << std::endl;
    return false;
  }

  PipelineNodeBuffer indicesBufferNode;
  indicesBufferNode.type = PIPELINE_NODE_BUFFER_INDEX;
  indicesBufferNode.buf.buffer = indexBuffer->GetBuffer();
  indicesBufferNode.buf.bufferSize = indexBuffer->GetBufferSize();

  cameraBuffer = std::make_shared<VkGPUBuffer>(renderer->GetGPUCtx());
  ret = cameraBuffer->AllocateAndBind(GPU_BUFFER_TYPE_UNIFORM,
                                      sizeof(DemoCameraUniform));
  if (ret != VK_SUCCESS) {
    Logger() << Logger::ERROR << "Failed to allocate GPU buffer" << std::endl;
    return false;
  }
  this->UpdateSceneState(0.0f);
  ret = cameraBuffer->UploadData(&this->cameraUniform,
                                 sizeof(this->cameraUniform));
  if (ret != VK_SUCCESS) {
    Logger() << Logger::ERROR << "Failed to upload GPU buffer" << std::endl;
    return false;
  }

  PipelineNodeBuffer cameraBufferNode;
  cameraBufferNode.type = PIPELINE_NODE_BUFFER_UNIFORM;
  cameraBufferNode.buf.buffer = cameraBuffer->GetBuffer();
  cameraBufferNode.buf.bufferSize = cameraBuffer->GetBufferSize();

  std::vector<PipelineNodeBuffer> buffers;
  buffers.push_back(vertexBufferNode);
  buffers.push_back(indicesBufferNode);
  buffers.push_back(cameraBufferNode);

  const std::function func = [](VkCommandBuffer commandBuffer) {
    // NA:
  };
  const GraphicsElement elementLeft{
      .pushConstantInfo = {.size = sizeof(DemoFrameInfo),
                           .data = &this->frameInfo},
      .buffers = buffers,
      .customDrawFunc = func,
  };
  this->graphicsPipelineNode->AddGraphicsElement(elementLeft);
  return true;
}

void GraphicsApp::Run() {
  while (running && !window->ShouldClose()) {
    window->PollEvent();
    window->Render();
    this->frameInfo.frameIndex++;
    this->UpdateSceneState(this->frameInfo.frameIndex / 60.0f);
    if (renderer->RenderFrame() != VK_SUCCESS) {
      return;
    }
    if (renderer->Present() != VK_SUCCESS) {
      return;
    }
  }
}

void GraphicsApp::CaptureFrame(const std::string &path) const {
  if (running && !window->ShouldClose()) {
    window->PollEvent();
    window->Render();
    renderer->RenderFrameOffScreen(path);
  }
}

void GraphicsApp::SetWindowSize(const uint32_t width, const uint32_t height) {
  this->windowWidth = width;
  this->windowHeight = height;
}

void GraphicsApp::SetWindowPos(const uint32_t x, const uint32_t y) {
  this->windowPosX = x;
  this->windowPosY = y;
}

void GraphicsApp::SetWindowTitle(const char *title) {
  this->windowTitle = title;
}
void GraphicsApp::SetWindowMode(const uint32_t mode) {
  this->windowMode = mode;
}

void GraphicsApp::OnKeyUp(const uint32_t key) {
  Logger() << "Key up: " << key << std::endl;
}

void GraphicsApp::OnKeyDown(const uint32_t key) {
  Logger() << "Key down: " << key << std::endl;
}

void GraphicsApp::OnMouseDown(const uint32_t key) {
  Logger() << "Mouse down: " << key << std::endl;
}

void GraphicsApp::OnMouseUp(const uint32_t key) {
  Logger() << "Mouse up: " << key << std::endl;
}

void GraphicsApp::OnMouseMove(const uint32_t x, const uint32_t y) {
  Logger() << "Mouse move: " << x << ", " << y << std::endl;
}
