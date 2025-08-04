//
// Created by neo on 25-4-27.
//

#include "RendererMesh.h"

#include "core/utils/ImageUtils.h"
#include "runtime/log/Log.h"

RendererMesh::RendererMesh(const std::shared_ptr<Mesh> &mesh) {
  this->mesh = mesh;
}
void RendererMesh::SetMaterial(const Material &material) const {
  this->mesh->material = material;
  this->materialBuffer->UploadData(&material, sizeof(Material));
}

void RendererMesh::SetTransformMatrix(const glm::mat4 &transform) const {
  this->mesh->transform = transform;
  this->transformMatrixBuffer->UploadData(&mesh->transform, sizeof(glm::mat4));
}

PipelineNodeBuffer RendererMesh::GetVertexBufferNode() const {
  PipelineNodeBuffer vertexBufferNode = {};
  vertexBufferNode.type = PIPELINE_NODE_BUFFER_VERTEX;
  vertexBufferNode.buf.bufferSize = vertexBuffer->GetBufferSize();
  vertexBufferNode.buf.buffer = vertexBuffer->GetBuffer();
  return vertexBufferNode;
}

PipelineNodeBuffer RendererMesh::GetIndicesBufferNode() const {
  PipelineNodeBuffer indicesBufferNode = {};
  indicesBufferNode.type = PIPELINE_NODE_BUFFER_INDEX;
  indicesBufferNode.buf.buffer = indicesBuffer->GetBuffer();
  indicesBufferNode.buf.bufferSize = indicesBuffer->GetBufferSize();
  return indicesBufferNode;
}

PipelineNodeBuffer RendererMesh::GetMaterialBufferNode() const {
  PipelineNodeBuffer materialBufferNode = {};
  materialBufferNode.type = PIPELINE_NODE_BUFFER_UNIFORM;
  materialBufferNode.buf.buffer = materialBuffer->GetBuffer();
  materialBufferNode.buf.bufferSize = materialBuffer->GetBufferSize();
  return materialBufferNode;
}

PipelineNodeBuffer RendererMesh::GetTransformMatrixBufferNode() const {
  PipelineNodeBuffer transformMatrixBufferNode = {};
  transformMatrixBufferNode.type = PIPELINE_NODE_BUFFER_UNIFORM;
  transformMatrixBufferNode.buf.buffer = transformMatrixBuffer->GetBuffer();
  transformMatrixBufferNode.buf.bufferSize =
      transformMatrixBuffer->GetBufferSize();
  return transformMatrixBufferNode;
}

PipelineNodeBuffer
RendererMesh::GetTextureBufferNode(const TextureType type) const {
  PipelineNodeBuffer textureBufferNode = {};
  if (!textures.contains(type)) {
    textureBufferNode.type = PIPELINE_NODE_UNKNOW;
    return textureBufferNode;
  }

  const std::shared_ptr<VkGPUTexture> texture = textures.at(type);

  textureBufferNode.type = PIPELINE_NODE_SAMPLER;
  textureBufferNode.sampler.image = texture->GetTextureImage();
  textureBufferNode.sampler.imageView = texture->GetTextureImageView();
  textureBufferNode.sampler.imageBuffer =
      texture->GetImageBuffer()->GetBuffer();
  textureBufferNode.sampler.imageBufferMemory =
      texture->GetImageBuffer()->GetDeviceMemory();
  textureBufferNode.sampler.imageLayout =
      VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
  textureBufferNode.sampler.sampler = texture->GetTextureSampler();
  return textureBufferNode;
}

bool RendererMesh::CreateVertex(const std::shared_ptr<VkGPUContext> &gpuCtx) {
  vertexBuffer = std::make_shared<VkGPUBuffer>(gpuCtx);
  if (vertexBuffer == nullptr) {
    Logger() << Logger::ERROR << "vertexBuffer is null" << std::endl;
    return false;
  }
  const VkDeviceSize vertexBufferSize =
      mesh->vertexData.size() * sizeof(Vertex);
  VkResult ret =
      vertexBuffer->AllocateAndBind(GPU_BUFFER_TYPE_VERTEX, vertexBufferSize);
  if (ret != VK_SUCCESS) {
    Logger() << Logger::ERROR << "Vertex buffer allocate and bind failed"
             << std::endl;
    return false;
  }
  ret = vertexBuffer->UploadData(mesh->vertexData.data(), vertexBufferSize);
  if (ret != VK_SUCCESS) {
    Logger() << Logger::ERROR << "Vertex buffer upload failed" << std::endl;
    return false;
  }
  return true;
}

bool RendererMesh::CreateIndices(const std::shared_ptr<VkGPUContext> &gpuCtx) {
  indicesBuffer = std::make_shared<VkGPUBuffer>(gpuCtx);
  if (indicesBuffer == nullptr) {
    Logger() << Logger::ERROR << "indexBuffer is null" << std::endl;
    return false;
  }

  const VkDeviceSize indicesBufferSize =
      mesh->indicesData.size() * sizeof(uint32_t);
  VkResult ret =
      indicesBuffer->AllocateAndBind(GPU_BUFFER_TYPE_INDEX, indicesBufferSize);
  if (ret != VK_SUCCESS) {
    Logger() << Logger::ERROR << "Index buffer allocate and bind failed"
             << std::endl;
    return false;
  }
  ret = indicesBuffer->UploadData(mesh->indicesData.data(), indicesBufferSize);
  if (ret != VK_SUCCESS) {
    Logger() << Logger::ERROR << "Index buffer upload failed" << std::endl;
    return false;
  }
  return true;
}
bool RendererMesh::CreateMaterial(const std::shared_ptr<VkGPUContext> &gpuCtx) {
  materialBuffer = std::make_shared<VkGPUBuffer>(gpuCtx);
  if (materialBuffer == nullptr) {
    Logger() << Logger::ERROR << "material is null" << std::endl;
    return false;
  }
  const VkDeviceSize materialBufferSize = sizeof(Material);
  VkResult ret = materialBuffer->AllocateAndBind(GPU_BUFFER_TYPE_UNIFORM,
                                                 materialBufferSize);
  if (ret != VK_SUCCESS) {
    Logger() << Logger::ERROR << "Material buffer allocate and bind failed"
             << std::endl;
    return false;
  }
  ret = materialBuffer->UploadData(&mesh->material, materialBufferSize);
  if (ret != VK_SUCCESS) {
    Logger() << Logger::ERROR << "Material buffer upload failed" << std::endl;
    return false;
  }
  return true;
}
bool RendererMesh::CreateTransform(
    const std::shared_ptr<VkGPUContext> &gpuCtx) {
  transformMatrixBuffer = std::make_shared<VkGPUBuffer>(gpuCtx);
  if (transformMatrixBuffer == nullptr) {
    Logger() << Logger::ERROR << "transform matrix is null" << std::endl;
    return false;
  }
  const VkDeviceSize transformMatrixBufferSize = sizeof(glm::mat4);
  VkResult ret = transformMatrixBuffer->AllocateAndBind(
      GPU_BUFFER_TYPE_UNIFORM, transformMatrixBufferSize);
  if (ret != VK_SUCCESS) {
    Logger() << Logger::ERROR
             << "transform matrix buffer allocate and bind failed" << std::endl;
    return false;
  }

  ret = transformMatrixBuffer->UploadData(&mesh->transform,
                                          transformMatrixBufferSize);
  if (ret != VK_SUCCESS) {
    Logger() << Logger::ERROR << "transform matrix buffer upload failed"
             << std::endl;
    return false;
  }
  return true;
}

bool RendererMesh::CreateTexture(const std::shared_ptr<VkGPUContext> &gpuCtx) {
  for (auto &[type, path, width, height, channels] : mesh->textures) {
    std::string texturePath = mesh->path + path;
    std::vector<char> data =
        ImageUtils::ReadPngFile(texturePath, &width, &height, &channels);
    Logger() << "Create texture " << string_TextureType(type) << ":"
             << mesh->path + path << ", width=" << width << ",height=" << height
             << ",channels=" << channels << std::endl;

    auto texture = std::make_shared<VkGPUTexture>(gpuCtx, width, height);
    VkResult result = texture->CreateTexture();
    if (result != VK_SUCCESS) {
      Logger() << Logger::ERROR << "Texture create failed" << std::endl;
      return false;
    }

    const auto buffer = texture->GetImageBuffer();
    result = buffer->UploadData(data.data(), data.size());
    if (result != VK_SUCCESS) {
      Logger() << Logger::ERROR << "Texture upload failed" << std::endl;
      return false;
    }

    this->textures.emplace(std::pair(type, texture));
  }
  return true;
}

bool RendererMesh::CreateGPUMesh(const std::shared_ptr<VkGPUContext> &gpuCtx) {
  /*
   * Vertex upload
   */
  if (!CreateVertex(gpuCtx)) {
    Logger() << Logger::ERROR << "Vertex create failed" << std::endl;
    return false;
  }

  /*
   * Indices upload
   */
  if (!CreateIndices(gpuCtx)) {
    Logger() << Logger::ERROR << "Index create failed" << std::endl;
    return false;
  }

  /*
   * Material upload
   */
  if (!CreateMaterial(gpuCtx)) {
    Logger() << Logger::ERROR << "Material create failed" << std::endl;
    return false;
  }

  /*
   * Transform matrix
   */
  if (!CreateTransform(gpuCtx)) {
    Logger() << Logger::ERROR << "Transform create failed" << std::endl;
    return false;
  }

  /*
   * Textures
   */
  if (!CreateTexture(gpuCtx)) {
    Logger() << Logger::ERROR << "Texture create failed" << std::endl;
    return false;
  }

  return true;
}

void RendererMesh::Destroy() {
  if (vertexBuffer != nullptr) {
    vertexBuffer->Destroy();
    vertexBuffer = nullptr;
  }
  if (indicesBuffer != nullptr) {
    indicesBuffer->Destroy();
    indicesBuffer = nullptr;
  }
  if (materialBuffer != nullptr) {
    materialBuffer->Destroy();
    materialBuffer = nullptr;
  }
  if (transformMatrixBuffer != nullptr) {
    transformMatrixBuffer->Destroy();
    transformMatrixBuffer = nullptr;
  }
}
