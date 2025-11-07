//
// Created by neo on 2025/2/9.
//

#include "GraphicsApp.h"

#include "3dgs/SpzModel.h"
#include "runtime/gpu/VkGPUHelper.h"
#include "runtime/log/Log.h"
#include "window/impl/GLFWWindowImpl.h"

#include <glm/ext/matrix_transform.hpp>

VkSurfaceKHR GetWindowSurface(const VkInstance instance) {
  const SurfaceGetParams params{.params = {
                                    .vkInstance = instance,
                                }};
  return window->GetSurface(params).surface.vkSurface;
}

bool GraphicsApp::ConstructRendererPipeline() {
  std::vector<VkVertexInputBindingDescription> vertexInputBindingDescriptions =
      {{.binding = 0,
        .stride = sizeof(Vertex),
        .inputRate = VK_VERTEX_INPUT_RATE_VERTEX}};
  std::vector<VkVertexInputAttributeDescription>
      vertexInputAttributeDescriptions = {};

  std::vector<VkDescriptorSetLayoutBinding> descriptorSetLayoutBindings;

  descriptorSetLayoutBindings.push_back(
      VkGPUHelper::BuildDescriptorSetLayoutBinding(
          0, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1,
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
      renderer->GetGPUCtx(), "GraphicsPipeline",
      this->renderer->GetMainRenderPass(), SHADER(rect_3dgs.vert.glsl.spv),
      SHADER(rect_3dgs.frag.glsl.spv), sizeof(FrameInfo),
      descriptorSetLayoutBindings, vertexInputBindingDescriptions,
      vertexInputAttributeDescriptions, this->windowWidth, this->windowHeight,
      viewport);
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

  const std::vector<GaussianPoint> points = model.getPoints();
  const auto pointsBuf = std::make_shared<VkGPUBuffer>(renderer->GetGPUCtx());
  VkResult ret = pointsBuf->AllocateAndBind(
      GPU_BUFFER_TYPE_STORAGE_SHARED, points.size() * sizeof(GaussianPoint));
  if (ret != VK_SUCCESS) {
    Logger() << Logger::ERROR << "Failed to allocate GPU buffer" << std::endl;
    return false;
  }
  ret = pointsBuf->UploadData(points.data(),
                              points.size() * sizeof(GaussianPoint));
  if (ret != VK_SUCCESS) {
    Logger() << Logger::ERROR << "Failed to upload GPU buffer" << std::endl;
    return false;
  }

  PipelineNodeBuffer pointsBufferNode;
  pointsBufferNode.type = PIPELINE_NODE_BUFFER_STORAGE_READ;
  pointsBufferNode.buf.buffer = pointsBuf->GetBuffer();
  pointsBufferNode.buf.bufferSize = pointsBuf->GetBufferSize();

  std::vector<PipelineNodeBuffer> buffersLeft;
  buffersLeft.push_back(pointsBufferNode);

  const std::function func = [](VkCommandBuffer commandBuffer) {
    // NA:
  };
  const GraphicsElement elementLeft{
      .pushConstantInfo = {.size = sizeof(FrameInfo), .data = &this->frameInfo},
      .buffers = buffersLeft,
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
