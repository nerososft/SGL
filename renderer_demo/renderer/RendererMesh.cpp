//
// Created by neo on 25-4-27.
//

#include "RendererMesh.h"

#include "gpu_engine/log/Log.h"

RendererMesh::RendererMesh(const std::vector<Vertex> &vertices,
                           const std::vector<uint32_t> &indices,
                           const Material &material,
                           const glm::mat4 &transform) {
    this->mesh.vertexData = vertices;
    this->mesh.indicesData = indices;
    this->mesh.material = material;
    this->mesh.transform = transform;
}

void RendererMesh::SetMaterial(const Material &material) {
    this->mesh.material = material;
    this->materialBuffer->UploadData(&material, sizeof(Material));
}

void RendererMesh::SetMvpMatrix(const MatrixMVP &mvpMatrix) {
    this->matrixMVP = mvpMatrix;
    this->mvpBuffer->UploadData(&matrixMVP, sizeof(MatrixMVP));
}

void RendererMesh::SetTransform(const glm::mat4 &transform) {
    this->mesh.transform = transform;
    this->matrixMVP.model = this->mesh.transform;
    this->mvpBuffer->UploadData(&matrixMVP, sizeof(MatrixMVP));
}

PipelineNodeBuffer RendererMesh::GetVertexBufferNode() const {
    PipelineNodeBuffer vertexBufferNode = {};
    vertexBufferNode.type = PIPELINE_NODE_BUFFER_VERTEX;
    vertexBufferNode.bufferSize = vertexBuffer->GetBufferSize();
    vertexBufferNode.buffer = vertexBuffer->GetBuffer();
    return vertexBufferNode;
}

PipelineNodeBuffer RendererMesh::GetIndicesBufferNode() const {
    PipelineNodeBuffer indicesBufferNode = {};
    indicesBufferNode.type = PIPELINE_NODE_BUFFER_INDEX;
    indicesBufferNode.buffer = indicesBuffer->GetBuffer();
    indicesBufferNode.bufferSize = indicesBuffer->GetBufferSize();
    return indicesBufferNode;
}

PipelineNodeBuffer RendererMesh::GetMaterialBufferNode() const {
    PipelineNodeBuffer materialBufferNode = {};
    materialBufferNode.type = PIPELINE_NODE_BUFFER_UNIFORM;
    materialBufferNode.buffer = materialBuffer->GetBuffer();
    materialBufferNode.bufferSize = materialBuffer->GetBufferSize();
    return materialBufferNode;
}

PipelineNodeBuffer RendererMesh::GetMVPBufferNode() const {
    PipelineNodeBuffer mvpBufferNode = {};
    mvpBufferNode.type = PIPELINE_NODE_BUFFER_UNIFORM;
    mvpBufferNode.buffer = mvpBuffer->GetBuffer();
    mvpBufferNode.bufferSize = mvpBuffer->GetBufferSize();
    return mvpBufferNode;
}

bool RendererMesh::CreateGPUMesh(const std::shared_ptr<VkGPUContext> &gpuCtx) {
    /*
     * Vertex upload
     */
    vertexBuffer = std::make_shared<VkGPUBuffer>(gpuCtx);
    if (vertexBuffer == nullptr) {
        Logger() << "vertexBuffer is null" << std::endl;
        return false;
    }
    const VkDeviceSize vertexBufferSize = mesh.vertexData.size() * sizeof(Vertex);
    VkResult ret = vertexBuffer->AllocateAndBind(GPU_BUFFER_TYPE_VERTEX, vertexBufferSize);
    if (ret != VK_SUCCESS) {
        Logger() << "Vertex buffer allocate and bind failed" << std::endl;
        return false;
    }
    ret = vertexBuffer->UploadData(mesh.vertexData.data(), vertexBufferSize);
    if (ret != VK_SUCCESS) {
        Logger() << "Vertex buffer upload failed" << std::endl;
        return false;
    }

    /*
     * Indices upload
     */
    indicesBuffer = std::make_shared<VkGPUBuffer>(gpuCtx);
    if (indicesBuffer == nullptr) {
        Logger() << "indexBuffer is null" << std::endl;
        return false;
    }

    const VkDeviceSize indicesBufferSize = mesh.indicesData.size() * sizeof(uint32_t);
    ret = indicesBuffer->AllocateAndBind(GPU_BUFFER_TYPE_INDEX, indicesBufferSize);
    if (ret != VK_SUCCESS) {
        Logger() << "Index buffer allocate and bind failed" << std::endl;
        return false;
    }
    ret = indicesBuffer->UploadData(mesh.indicesData.data(), indicesBufferSize);
    if (ret != VK_SUCCESS) {
        Logger() << "Index buffer upload failed" << std::endl;
        return false;
    }

    /*
     * Material upload
     */
    materialBuffer = std::make_shared<VkGPUBuffer>(gpuCtx);
    if (materialBuffer == nullptr) {
        Logger() << "material is null" << std::endl;
        return false;
    }
    const VkDeviceSize materialBufferSize = sizeof(Material);
    ret = materialBuffer->AllocateAndBind(GPU_BUFFER_TYPE_UNIFORM, materialBufferSize);
    if (ret != VK_SUCCESS) {
        Logger() << "Material buffer allocate and bind failed" << std::endl;
        return false;
    }
    ret = materialBuffer->UploadData(&mesh.material, materialBufferSize);
    if (ret != VK_SUCCESS) {
        Logger() << "Material buffer upload failed" << std::endl;
        return false;
    }

    /*
     * MVP matrix
     */
    mvpBuffer = std::make_shared<VkGPUBuffer>(gpuCtx);
    if (materialBuffer == nullptr) {
        Logger() << "mvp is null" << std::endl;
        return false;
    }
    const VkDeviceSize mvpBufferSize = sizeof(MatrixMVP);
    ret = mvpBuffer->AllocateAndBind(GPU_BUFFER_TYPE_UNIFORM, mvpBufferSize);
    if (ret != VK_SUCCESS) {
        Logger() << "MVP buffer allocate and bind failed" << std::endl;
        return false;
    }

    ret = mvpBuffer->UploadData(&matrixMVP, mvpBufferSize);
    if (ret != VK_SUCCESS) {
        Logger() << "MVP buffer upload failed" << std::endl;
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
    if (mvpBuffer != nullptr) {
        mvpBuffer->Destroy();
        mvpBuffer = nullptr;
    }
}
