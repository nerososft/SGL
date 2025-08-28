//
// Created by neo on 2025/2/9.
//

#include "GraphicsApp.h"
#include "runtime/gpu/VkGPUHelper.h"
#include "runtime/log/Log.h"
#include "runtime/utils/TimeUtils.h"
#include "scene/ModelLoader.h"
#include "window/impl/GLFWWindowImpl.h"

#include <glm/ext/matrix_transform.hpp>

VkSurfaceKHR GetWindowSurface(const VkInstance instance) {
  const SurfaceGetParams params{.params = {
                                    .vkInstance = instance,
                                }};
  return window->GetSurface(params).surface.vkSurface;
}

bool GraphicsApp::ConstructLeftRendererPipeline() {
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
          0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1,
          VK_SHADER_STAGE_ALL_GRAPHICS));
  descriptorSetLayoutBindings.push_back(
      VkGPUHelper::BuildDescriptorSetLayoutBinding(
          1, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1,
          VK_SHADER_STAGE_ALL_GRAPHICS));
  descriptorSetLayoutBindings.push_back(
      VkGPUHelper::BuildDescriptorSetLayoutBinding(
          2, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1,
          VK_SHADER_STAGE_ALL_GRAPHICS));
  descriptorSetLayoutBindings.push_back(
      VkGPUHelper::BuildDescriptorSetLayoutBinding(
          3, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1,
          VK_SHADER_STAGE_ALL_GRAPHICS));
  descriptorSetLayoutBindings.push_back(
      VkGPUHelper::BuildDescriptorSetLayoutBinding(
          4, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1,
          VK_SHADER_STAGE_ALL_GRAPHICS));
  descriptorSetLayoutBindings.push_back(
      VkGPUHelper::BuildDescriptorSetLayoutBinding(
          5, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1,
          VK_SHADER_STAGE_ALL_GRAPHICS));
  descriptorSetLayoutBindings.push_back(
      VkGPUHelper::BuildDescriptorSetLayoutBinding(
          6, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1,
          VK_SHADER_STAGE_ALL_GRAPHICS));
  descriptorSetLayoutBindings.push_back(
      VkGPUHelper::BuildDescriptorSetLayoutBinding(
          7, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1,
          VK_SHADER_STAGE_ALL_GRAPHICS));
  descriptorSetLayoutBindings.push_back(
      VkGPUHelper::BuildDescriptorSetLayoutBinding(
          8, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1,
          VK_SHADER_STAGE_ALL_GRAPHICS));
  descriptorSetLayoutBindings.push_back(
      VkGPUHelper::BuildDescriptorSetLayoutBinding(
          9, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1,
          VK_SHADER_STAGE_ALL_GRAPHICS));
  descriptorSetLayoutBindings.push_back(
      VkGPUHelper::BuildDescriptorSetLayoutBinding(
          10, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1,
          VK_SHADER_STAGE_ALL_GRAPHICS));

  VkViewport viewport{
      .x = 0,
      .y = 0,
      .width = static_cast<float>(windowWidth) / 2,
      .height = static_cast<float>(windowHeight),
      .minDepth = 0,
      .maxDepth = 1,
  };
  this->graphicsPipelineLeftNode = std::make_shared<GraphicsPipelineNode>(
      renderer->GetGPUCtx(), "leftGraphicsPipeline",
      this->renderer->GetMainRenderPass(), SHADER(rect.vert.glsl.spv),
      SHADER(rect.frag.glsl.spv), sizeof(FrameInfo),
      descriptorSetLayoutBindings, vertexInputBindingDescriptions,
      vertexInputAttributeDescriptions, this->windowWidth, this->windowHeight,
      viewport);
  if (this->graphicsPipelineLeftNode == nullptr) {
    Logger() << Logger::ERROR << "Failed to create graphics pipeline node!"
             << std::endl;
    return false;
  }

  const VkResult ret = this->graphicsPipelineLeftNode->CreateComputeGraphNode();
  if (ret != VK_SUCCESS) {
    Logger() << Logger::ERROR << "Failed to create graphics pipeline node!"
             << std::endl;
    return false;
  }

  renderer->AddRenderGraph(this->graphicsPipelineLeftNode);

  return true;
}
bool GraphicsApp::ConstructRightRendererPipeline() {
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
          0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1,
          VK_SHADER_STAGE_ALL_GRAPHICS));
  descriptorSetLayoutBindings.push_back(
      VkGPUHelper::BuildDescriptorSetLayoutBinding(
          1, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1,
          VK_SHADER_STAGE_ALL_GRAPHICS));
  descriptorSetLayoutBindings.push_back(
      VkGPUHelper::BuildDescriptorSetLayoutBinding(
          2, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1,
          VK_SHADER_STAGE_ALL_GRAPHICS));
  descriptorSetLayoutBindings.push_back(
      VkGPUHelper::BuildDescriptorSetLayoutBinding(
          3, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1,
          VK_SHADER_STAGE_ALL_GRAPHICS));
  descriptorSetLayoutBindings.push_back(
      VkGPUHelper::BuildDescriptorSetLayoutBinding(
          4, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1,
          VK_SHADER_STAGE_ALL_GRAPHICS));
  descriptorSetLayoutBindings.push_back(
      VkGPUHelper::BuildDescriptorSetLayoutBinding(
          5, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1,
          VK_SHADER_STAGE_ALL_GRAPHICS));
  descriptorSetLayoutBindings.push_back(
      VkGPUHelper::BuildDescriptorSetLayoutBinding(
          6, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1,
          VK_SHADER_STAGE_ALL_GRAPHICS));
  descriptorSetLayoutBindings.push_back(
      VkGPUHelper::BuildDescriptorSetLayoutBinding(
          7, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1,
          VK_SHADER_STAGE_ALL_GRAPHICS));
  descriptorSetLayoutBindings.push_back(
      VkGPUHelper::BuildDescriptorSetLayoutBinding(
          8, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1,
          VK_SHADER_STAGE_ALL_GRAPHICS));
  descriptorSetLayoutBindings.push_back(
      VkGPUHelper::BuildDescriptorSetLayoutBinding(
          9, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1,
          VK_SHADER_STAGE_ALL_GRAPHICS));
  descriptorSetLayoutBindings.push_back(
      VkGPUHelper::BuildDescriptorSetLayoutBinding(
          10, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1,
          VK_SHADER_STAGE_ALL_GRAPHICS));

  VkViewport viewport{
      .x = static_cast<float>(windowWidth) / 2,
      .y = 0,
      .width = static_cast<float>(windowWidth) / 2,
      .height = static_cast<float>(windowHeight),
      .minDepth = 0,
      .maxDepth = 1,
  };
  this->graphicsPipelineRightNode = std::make_shared<GraphicsPipelineNode>(
      renderer->GetGPUCtx(), "rightGraphicsPipeline",
      this->renderer->GetMainRenderPass(), SHADER(rect.vert.glsl.spv),
      SHADER(rect.frag.glsl.spv), sizeof(FrameInfo),
      descriptorSetLayoutBindings, vertexInputBindingDescriptions,
      vertexInputAttributeDescriptions, this->windowWidth, this->windowHeight,
      viewport);
  if (this->graphicsPipelineRightNode == nullptr) {
    Logger() << Logger::ERROR << "Failed to create graphics pipeline node!"
             << std::endl;
    return false;
  }

  const VkResult ret =
      this->graphicsPipelineRightNode->CreateComputeGraphNode();
  if (ret != VK_SUCCESS) {
    Logger() << Logger::ERROR << "Failed to create graphics pipeline node!"
             << std::endl;
    return false;
  }

  renderer->AddRenderGraph(this->graphicsPipelineRightNode);

  return true;
}

bool GraphicsApp::InitCameras() {
  cameraLeft = std::make_shared<RendererCamera>(glm::vec3(-0.15, 2, 0),
                                                glm::vec3(0, 0, -1));
  if (!cameraLeft->CreateGPUCamera(
          renderer->GetGPUCtx(), (static_cast<float>(this->windowWidth) / 2) /
                                     static_cast<float>(this->windowHeight))) {
    return false;
  }

  cameraRight = std::make_shared<RendererCamera>(glm::vec3(0.15, 2, 0),
                                                 glm::vec3(0, 0, -1));
  if (!cameraRight->CreateGPUCamera(
          renderer->GetGPUCtx(), (static_cast<float>(this->windowWidth) / 2) /
                                     static_cast<float>(this->windowHeight))) {
    return false;
  }
  return true;
}

bool GraphicsApp::InitLights() {
  const auto light = std::make_shared<RendererLight>();
  if (!light->CreateGPULight(renderer->GetGPUCtx())) {
    return false;
  }
  light->SetLightPosition(glm::vec4(1, 5, 1, 0));
  light->SetLightColor(glm::vec4(1.5f, 1.5f, 1.5f, 1.0f));
  this->rendererLights.push_back(light);
  return true;
}

void GraphicsApp::Init() {
  window = std::make_shared<GLFWWindowImpl>(this);
  const std::vector<const char *> requiredExtensions =
      window->GetRendererRequiredExtensions();
  renderer = std::make_shared<Renderer>(this->windowWidth, this->windowHeight);

  window->CreateWindow(this->windowPosX, this->windowPosY, this->windowWidth,
                       this->windowHeight, this->windowTitle, this->windowMode);
  if (!renderer->Init(requiredExtensions, GetWindowSurface)) {
    Logger() << Logger::ERROR << "Failed to initialize renderer" << std::endl;
    return;
  }

  if (!InitCameras()) {
    Logger() << Logger::ERROR << "Failed to initialize camera" << std::endl;
    return;
  }

  if (!InitLights()) {
    Logger() << Logger::ERROR << "Failed to initialize lights" << std::endl;
    return;
  }

  if (!this->ConstructLeftRendererPipeline() ||
      !ConstructRightRendererPipeline()) {
    Logger() << Logger::ERROR << "Failed to construct renderer" << std::endl;
    return;
  }

  const std::vector<std::shared_ptr<Mesh>> models = ModelLoader::LoadModel(
      "../../../examples/renderer_demo/assets/builtin.models/Helmet/",
      "DamagedHelmet.gltf");
  for (auto &mesh : models) {
    const auto renderMesh = std::make_shared<RendererMesh>(mesh);
    if (!renderMesh->CreateGPUMesh(renderer->GetGPUCtx())) {
      Logger() << Logger::ERROR << "Failed to create GPUMesh!" << std::endl;
      return;
    }
    this->rendererMeshes.push_back(renderMesh);

    std::vector<PipelineNodeBuffer> buffersLeft;
    buffersLeft.push_back(renderMesh->GetVertexBufferNode());
    buffersLeft.push_back(renderMesh->GetIndicesBufferNode());
    buffersLeft.push_back(renderMesh->GetMaterialBufferNode()); // uniform 0
    buffersLeft.push_back(
        renderMesh->GetTransformMatrixBufferNode()); // uniform 1
    buffersLeft.push_back(
        cameraLeft->GetViewProjectionMatrixBufferNode());           // uniform 2
    buffersLeft.push_back(rendererLights[0]->GetLightBufferNode()); // uniform 3
    buffersLeft.push_back(
        renderMesh->GetTextureBufferNode(TextureType_DIFFUSE)); // sampler 4
    buffersLeft.push_back(
        renderMesh->GetTextureBufferNode(TextureType_EMISSIVE)); // sampler 5
    buffersLeft.push_back(
        renderMesh->GetTextureBufferNode(TextureType_NORMALS)); // sampler 6
    buffersLeft.push_back(
        renderMesh->GetTextureBufferNode(TextureType_LIGHTMAP)); // sampler 7
    buffersLeft.push_back(
        renderMesh->GetTextureBufferNode(TextureType_BASE_COLOR)); // sampler 8
    buffersLeft.push_back(
        renderMesh->GetTextureBufferNode(TextureType_METALNESS)); // sampler 9
    buffersLeft.push_back(renderMesh->GetTextureBufferNode(
        TextureType_DIFFUSE_ROUGHNESS)); // sampler 10

    const std::function func = [](VkCommandBuffer commandBuffer) {
      // NA:
    };
    const GraphicsElement elementLeft{
        .pushConstantInfo = {.size = sizeof(FrameInfo),
                             .data = &this->frameInfo},
        .buffers = buffersLeft,
        .customDrawFunc = func,
    };
    this->graphicsPipelineLeftNode->AddGraphicsElement(elementLeft);

    std::vector<PipelineNodeBuffer> buffersRight;
    buffersRight.push_back(renderMesh->GetVertexBufferNode());
    buffersRight.push_back(renderMesh->GetIndicesBufferNode());
    buffersRight.push_back(renderMesh->GetMaterialBufferNode()); // uniform 0
    buffersRight.push_back(
        renderMesh->GetTransformMatrixBufferNode()); // uniform 1
    buffersRight.push_back(
        cameraRight->GetViewProjectionMatrixBufferNode()); // uniform 2
    buffersRight.push_back(
        rendererLights[0]->GetLightBufferNode()); // uniform 3
    buffersRight.push_back(
        renderMesh->GetTextureBufferNode(TextureType_DIFFUSE)); // sampler 4
    buffersRight.push_back(
        renderMesh->GetTextureBufferNode(TextureType_EMISSIVE)); // sampler 5
    buffersRight.push_back(
        renderMesh->GetTextureBufferNode(TextureType_NORMALS)); // sampler 6
    buffersRight.push_back(
        renderMesh->GetTextureBufferNode(TextureType_LIGHTMAP)); // sampler 7
    buffersRight.push_back(
        renderMesh->GetTextureBufferNode(TextureType_BASE_COLOR)); // sampler 8
    buffersRight.push_back(
        renderMesh->GetTextureBufferNode(TextureType_METALNESS)); // sampler 9
    buffersRight.push_back(renderMesh->GetTextureBufferNode(
        TextureType_DIFFUSE_ROUGHNESS)); // sampler 10

    const GraphicsElement elementRight{
        .pushConstantInfo = {.size = sizeof(FrameInfo),
                             .data = &this->frameInfo},
        .buffers = buffersRight,
        .customDrawFunc = func,
    };
    this->graphicsPipelineRightNode->AddGraphicsElement(elementRight);
  }

  this->lastRenderTimeMs = TimeUtils::GetCurrentMonoMs();
}
void GraphicsApp::Update() const {
  for (const auto& light : rendererLights) {
    glm::vec4 pos =
        glm::rotate(glm::mat4(1.0f), glm::radians(-0.1f), glm::vec3(0, 0, 1)) *
        light->GetLightPosition();
    light->SetLightPosition(pos);
  }

  glm::mat4 viewLeft = cameraLeft->GetViewMatrix();
  viewLeft = glm::rotate(viewLeft, glm::radians(0.1f), glm::vec3(0, 0, 1));
  cameraLeft->SetViewMatrix(viewLeft);

  glm::mat4 viewRight = cameraRight->GetViewMatrix();
  viewRight = glm::rotate(viewRight, glm::radians(0.1f), glm::vec3(0, 0, 1));
  cameraRight->SetViewMatrix(viewRight);
}

void GraphicsApp::Run() {
  while (running && !window->ShouldClose()) {
    window->PollEvent();
    window->Render();
    Update();
    this->frameInfo.frameIndex++;
    if (renderer->RenderFrame() != VK_SUCCESS) {
      return;
    }
    const uint64_t renderTimeMs = TimeUtils::GetCurrentMonoMs();
    if (renderTimeMs - this->lastRenderTimeMs > 1000) {
      this->fps = this->frameInfo.frameIndex - this->lastRenderFrame;
      this->lastRenderFrame = this->frameInfo.frameIndex;
      this->lastRenderTimeMs = renderTimeMs;
    }

    if (renderer->Present() != VK_SUCCESS) {
      return;
    }
  }
}

uint64_t GraphicsApp::GetFPS() const { return this->fps; }

void GraphicsApp::CaptureFrame(const std::string &path) const {
  if (running && !window->ShouldClose()) {
    window->PollEvent();
    window->Render();
    Update();
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
  switch (key) {
  case GLFW_KEY_W:
  case GLFW_KEY_UP:
    cameraLeft->MoveForward();
    cameraRight->MoveForward();
    break;
  case GLFW_KEY_S:
  case GLFW_KEY_DOWN:
    cameraLeft->MoveBackward();
    cameraRight->MoveBackward();
    break;
  case GLFW_KEY_A:
  case GLFW_KEY_LEFT:
    cameraLeft->MoveLeft();
    cameraRight->MoveLeft();
    break;
  case GLFW_KEY_D:
  case GLFW_KEY_RIGHT:
    cameraLeft->MoveRight();
    cameraRight->MoveRight();
    break;
  default:
    break;
  }

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
