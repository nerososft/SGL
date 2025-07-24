//
// Created by neo on 25-5-25.
//

#include "Matrix.h"

#include "core/log/Log.h"

Matrix::Matrix(const uint32_t width, const uint32_t height) {
  this->width = width;
  this->height = height;
  this->buffer = nullptr;
}

Matrix::Matrix(const uint32_t width, const uint32_t height,
               const std::shared_ptr<VkGPUBuffer> &buffer) {
  this->width = width;
  this->height = height;
  this->buffer = buffer;
}

Matrix::~Matrix() { this->Destroy(); }

bool Matrix::CreateMatrix(std::shared_ptr<VkGPUContext> &ctx) {
  if (this->buffer != nullptr) {
    return true;
  }
  this->buffer = std::make_shared<VkGPUBuffer>(ctx);
  if (this->buffer == nullptr) {
    Logger() << "failed to create GPU buffer" << std::endl;
    return false;
  }
  if (this->buffer->AllocateAndBind(GPU_BUFFER_TYPE_STORAGE_SHARED,
                                    this->width * this->height *
                                        sizeof(float)) != VK_SUCCESS) {
    Logger() << "failed to allocate GPU buffer" << std::endl;
    return false;
  }
  return true;
}

bool Matrix::SetData(const std::vector<float> &data) const {
  if (this->buffer == nullptr) {
    Logger() << "buffer not allocated" << std::endl;
    return false;
  }

  if (data.size() > (this->width * this->height)) {
    Logger() << "data size " << data.size()
             << " is larger than GPU buffer size " << this->width * this->height
             << std::endl;
    return false;
  }

  if (this->buffer->UploadData(data.data(), data.size() * sizeof(float)) !=
      VK_SUCCESS) {
    Logger() << "failed to upload data" << std::endl;
    return false;
  }

  return true;
}

std::shared_ptr<VkGPUBuffer> Matrix::GetBuffer() { return this->buffer; }

void *Matrix::GetDataAddr() const {
  if (this->buffer == nullptr) {
    Logger() << "buffer not allocated" << std::endl;
    return nullptr;
  }
  if (this->buffer->MapBuffer() != VK_SUCCESS) {
    Logger() << "failed to map buffer" << std::endl;
    return nullptr;
  }
  return this->buffer->GetMappedAddr();
}

void Matrix::Print() const {
  const float *data = static_cast<float *>(this->GetDataAddr());
  std::cout << "(" << this->GetWidth() << ", " << this->GetHeight() << ")";
  std::cout << "[";
  for (size_t y = 0; y < this->height; y++) {
    std::cout << "[";
    for (size_t x = 0; x < this->width; x++) {
      std::cout << data[y * this->width + x]
                << (x == this->width - 1 ? "" : ",");
    }
    if (y == this->height - 1) {
      std::cout << "]";
    } else {
      std::cout << "]," << std::endl;
    }
  }
  std::cout << "]" << std::endl;
  this->buffer->UnMapBuffer();
}

void Matrix::Destroy() const {
  if (this->buffer != nullptr) {
    this->buffer->Destroy();
  }
}
