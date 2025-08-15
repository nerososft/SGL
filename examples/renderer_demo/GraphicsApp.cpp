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

  this->graphicsPipelineNode = std::make_shared<GraphicsPipelineNode>(
      renderer->GetGPUCtx(), "mainGraphicsPipeline",
      this->renderer->GetMainRenderPass(), SHADER(rect.vert.glsl.spv),
      SHADER(rect.frag.glsl.spv), sizeof(FrameInfo),
      descriptorSetLayoutBindings, vertexInputBindingDescriptions,
      vertexInputAttributeDescriptions, this->windowWidth, this->windowHeight);
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

bool GraphicsApp::InitCamera() {
  camera =
      std::make_shared<RendererCamera>(glm::vec3(0, 5, 0), glm::vec3(0, 0, -1));
  if (!camera->CreateGPUCamera(renderer->GetGPUCtx(),
                               this->windowWidth / this->windowHeight)) {
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
                       this->windowHeight, this->windowTitle);
  if (!renderer->Init(requiredExtensions, GetWindowSurface)) {
    Logger() << Logger::ERROR << "Failed to initialize renderer" << std::endl;
    return;
  }

  if (!InitCamera()) {
    Logger() << Logger::ERROR << "Failed to initialize camera" << std::endl;
    return;
  }

  if (!InitLights()) {
    Logger() << Logger::ERROR << "Failed to initialize lights" << std::endl;
    return;
  }

  if (!this->ConstructRendererPipeline()) {
    Logger() << Logger::ERROR << "Failed to construct renderer" << std::endl;
    return;
  }

  const std::vector<std::shared_ptr<Mesh>> models = ModelLoader::LoadModel(
      "../../../examples/renderer_demo/assets/builtin.models/Helmet/",
      "DamagedHelmet.gltf");
  for (auto &mesh : models) {
    std::vector<PipelineNodeBuffer> buffers;

    const auto renderMesh = std::make_shared<RendererMesh>(mesh);
    if (!renderMesh->CreateGPUMesh(renderer->GetGPUCtx())) {
      Logger() << Logger::ERROR << "Failed to create GPUMesh!" << std::endl;
      return;
    }
    this->rendererMeshes.push_back(renderMesh);

    buffers.push_back(renderMesh->GetVertexBufferNode());
    buffers.push_back(renderMesh->GetIndicesBufferNode());
    buffers.push_back(renderMesh->GetMaterialBufferNode());         // uniform 0
    buffers.push_back(renderMesh->GetTransformMatrixBufferNode());  // uniform 1
    buffers.push_back(camera->GetViewProjectionMatrixBufferNode()); // uniform 2
    buffers.push_back(rendererLights[0]->GetLightBufferNode());     // uniform 3
    buffers.push_back(
        renderMesh->GetTextureBufferNode(TextureType_BASE_COLOR)); // sampler 4

    const GraphicsElement element{
        .pushConstantInfo = {.size = sizeof(FrameInfo),
                             .data = &this->frameInfo},
        .buffers = buffers,
        .customDrawFunc = nullptr,
    };

    this->graphicsPipelineNode->AddGraphicsElement(element);
  }

  this->lastRenderTimeMs = TimeUtils::GetCurrentMonoMs();
}
void GraphicsApp::Update() const {
  for (const auto light : rendererLights) {
    glm::vec4 pos =
        glm::rotate(glm::mat4(1.0f), glm::radians(-0.1f), glm::vec3(0, 0, 1)) *
        light->GetLightPosition();
    light->SetLightPosition(pos);
  }

  glm::mat4 view = camera->GetViewMatrix();
  view = glm::rotate(view, glm::radians(0.1f), glm::vec3(0, 0, 1));
  camera->SetViewMatrix(view);
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

void GraphicsApp::OnKeyUp(const uint32_t key) {
  Logger() << "Key up: " << key << std::endl;
}

void GraphicsApp::OnKeyDown(const uint32_t key) {
  switch (key) {
  case GLFW_KEY_W:
  case GLFW_KEY_UP:
    camera->MoveForward();
    break;
  case GLFW_KEY_S:
  case GLFW_KEY_DOWN:
    camera->MoveBackward();
    break;
  case GLFW_KEY_A:
  case GLFW_KEY_LEFT:
    camera->MoveLeft();
    break;
  case GLFW_KEY_D:
  case GLFW_KEY_RIGHT:
    camera->MoveRight();
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
