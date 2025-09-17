//
// Created by neo on 2025/2/9.
//

#include "GraphicsApp.h"
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
      vertexInputAttributeDescriptions = {
          {
              .location = 0,
              .binding = 0,
              .format = VK_FORMAT_R32G32B32_SFLOAT,
              .offset = offsetof(Vertex, position),
          },
          {
              .location = 1,
              .binding = 0,
              .format = VK_FORMAT_R32G32B32_SFLOAT,
              .offset = offsetof(Vertex, color),
          },
          {
              .location = 2,
              .binding = 0,
              .format = VK_FORMAT_R32G32B32_SFLOAT,
              .offset = offsetof(Vertex, normal),
          },
          {
              .location = 3,
              .binding = 0,
              .format = VK_FORMAT_R32G32_SFLOAT,
              .offset = offsetof(Vertex, texCoords),
          },
      };

  std::vector<VkDescriptorSetLayoutBinding> descriptorSetLayoutBindings;

  descriptorSetLayoutBindings.push_back(
      VkGPUHelper::BuildDescriptorSetLayoutBinding(
          0, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1,
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
      this->renderer->GetMainRenderPass(), SHADER(rect_2d.vert.glsl.spv),
      SHADER(rect_2d.frag.glsl.spv), sizeof(FrameInfo),
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

  auto mesh = std::make_shared<Mesh>();

  mesh->vertexData = {{
                          .position = {-1.0f, 1.0f, 0},
                          .texCoords = {0, 1},
                      },
                      {
                          .position = {-1.0f, -1.0f, 0},
                          .texCoords = {0, 0},
                      },
                      {
                          .position = {1.0f, -1.0f, 0},
                          .texCoords = {1, 0},
                      },
                      {
                          .position = {1.0f, -1.0f, 0},
                          .texCoords = {1, 0},
                      },
                      {
                          .position = {1.0f, 1.0f, 0},
                          .texCoords = {1, 1},
                      },
                      {
                          .position = {-1.0f, 1.0f, 0},
                          .texCoords = {0, 1},
                      }};
  mesh->indicesData = {0, 1, 2, 3, 4, 5};
  mesh->textures = {{.type = TextureType_BASE_COLOR,
                     .path = "../../../examples/renderer_demo_2d/assets/"
                             "builtin.textures/colorful.jpg",
                     .width = 3840,
                     .height = 2556,
                     .channels = 4}};

  const auto renderMesh = std::make_shared<RendererMesh>(mesh);
  if (!renderMesh->CreateGPUMesh(renderer->GetGPUCtx())) {
    Logger() << Logger::ERROR << "Failed to create GPUMesh!" << std::endl;
    return false;
  }
  this->rendererMeshes.push_back(renderMesh);

  std::vector<PipelineNodeBuffer> buffersLeft;
  buffersLeft.push_back(renderMesh->GetVertexBufferNode());
  buffersLeft.push_back(renderMesh->GetIndicesBufferNode());
  buffersLeft.push_back(
      renderMesh->GetTextureBufferNode(TextureType_BASE_COLOR)); // sampler 1

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
