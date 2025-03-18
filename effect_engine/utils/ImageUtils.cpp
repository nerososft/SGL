//
// Created by neo on 2025/3/10.
//

#include "ImageUtils.h"

#include <fstream>
#include <iostream>

#include "png.h"
#include "TimeUtils.h"
#include "effect_engine/log/Log.h"

typedef struct {
    char *data;
    size_t size;
    size_t offset;
} PngReadBuf;

void png_read_func(png_structp png, png_bytep outBuffer, size_t bufferToRead) {
    auto *buf = static_cast<PngReadBuf *>(png_get_io_ptr(png));
    if (buf->offset + bufferToRead > buf->size) {
        std::cerr << "png_read_func: out of buffer" << std::endl;
        return;
    }
    memcpy(outBuffer, buf->data + buf->offset, bufferToRead);
    buf->offset += bufferToRead;
}

void png_write_func(png_structp png, png_bytep inBuffer, size_t bufferToWrite) {
    auto *ofs = static_cast<std::ofstream *>(png_get_io_ptr(png));
    if (!ofs->good()) {
        std::cerr << "png_write_func: bad ofstream" << std::endl;
        return;
    }
    ofs->write(reinterpret_cast<char *>(inBuffer), bufferToWrite);
    if (!ofs->good()) {
        std::cerr << "png_write_func: bad ofstream" << std::endl;
    }
}

void png_flush_func(png_structp png) {
    auto *ofs = static_cast<std::ofstream *>(png_get_io_ptr(png));
    if (ofs->good()) {
        ofs->flush();
    }
}

std::vector<char> ImageUtils::ReadPngFile(const std::string &fileName,
                                          uint32_t *imageWidth,
                                          uint32_t *imageHeight,
                                          uint32_t *channel) {
    Logger() << "Reading PNG file " << fileName << std::endl;
    const uint64_t pngReadStart = TimeUtils::GetCurrentMonoMs();
    std::vector<char> data;

    std::ifstream file;
    file.open(fileName, std::ios::binary | std::ios::in | std::ios::ate);
    if (!file.is_open()) {
        std::cerr << "Failed to open file " << fileName << std::endl;
        return data;
    }

    std::vector<char> pngData;
    const size_t fileSize = file.tellg();
    pngData.resize(fileSize);
    file.seekg(0);
    file.read(pngData.data(), fileSize);

    png_structp png = png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
    if (!png) {
        std::cerr << "Failed to create png struct" << std::endl;
        return data;
    }
    const png_infop info = png_create_info_struct(png);
    if (!info) {
        std::cerr << "Failed to create png info" << std::endl;
        png_destroy_read_struct(&png, nullptr, nullptr);
        return data;
    }
    if (setjmp(png_jmpbuf(png))) {
        png_destroy_read_struct(&png, nullptr, nullptr);
        return data;
    }

    PngReadBuf readBuf{
        .data = pngData.data(), .size = fileSize, .offset = 0,
    };
    png_set_read_fn(png, &readBuf, png_read_func);

    png_read_info(png, info);

    *imageWidth = png_get_image_width(png, info);
    *imageHeight = png_get_image_height(png, info);
    *channel = png_get_channels(png, info);

    const png_byte color_type = png_get_color_type(png, info);
    const png_byte bit_depth = png_get_bit_depth(png, info);

    if (bit_depth == 16) {
        png_set_strip_16(png);
    }

    if (color_type == PNG_COLOR_TYPE_PALETTE) {
        png_set_palette_to_rgb(png);
    }

    if (color_type == PNG_COLOR_TYPE_GRAY || bit_depth < 8) {
        png_set_expand_gray_1_2_4_to_8(png);
    }

    if (png_get_valid(png, info, PNG_INFO_tRNS)) {
        png_set_tRNS_to_alpha(png);
    }

    if (color_type != PNG_COLOR_TYPE_RGBA) {
        png_set_filler(png, 0xFF, PNG_FILLER_AFTER);
    }
    *channel = 4;
    png_read_update_info(png, info);

    const size_t rowBytes = png_get_rowbytes(png, info);
    for (size_t row = 0; row < *imageHeight; row++) {
        auto *rowPixels = static_cast<unsigned char *>(malloc(rowBytes));
        if (rowPixels == nullptr) {
            std::cerr << "Failed to allocate row pointer" << std::endl;
            png_destroy_read_struct(&png, nullptr, nullptr);
            return data;
        }
        png_read_row(png, rowPixels, nullptr);
        for (size_t col = 0; col < rowBytes; col++) {
            data.push_back(rowPixels[col]);
        }
        free(rowPixels);
    }

    const uint64_t pngReadEnd = TimeUtils::GetCurrentMonoMs();
    Logger() << "Read PNG Usage:" << pngReadEnd - pngReadStart << "ms" << std::endl;
    return data;
}

void ImageUtils::WritePngFile(const std::string &fileName,
                              const uint32_t imageWidth,
                              const uint32_t imageHeight,
                              const uint32_t channel,
                              const void *imageData) {
    Logger() << "Writing PNG file " << fileName << std::endl;
    const uint64_t pngWriteStart = TimeUtils::GetCurrentMonoMs();
    png_structp png = png_create_write_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
    if (!png) {
        std::cerr << "Failed to create png struct" << std::endl;
        return;
    }
    png_infop info = png_create_info_struct(png);
    if (!info) {
        std::cerr << "Failed to create png info" << std::endl;
        png_destroy_write_struct(&png, nullptr);
        return;
    }
    if (setjmp(png_jmpbuf(png))) {
        png_destroy_write_struct(&png, &info);
        return;
    }

    std::ofstream file;
    file.open(fileName, std::ios::binary | std::ios::out | std::ios::app);
    if (!file.is_open()) {
        std::cerr << "Failed to open file " << fileName << std::endl;
        png_destroy_write_struct(&png, &info);
        return;
    }

    png_set_write_fn(png, &file, png_write_func, png_flush_func);

    png_set_IHDR(png,
                 info,
                 imageWidth,
                 imageHeight,
                 8,
                 PNG_COLOR_TYPE_RGBA,
                 PNG_INTERLACE_NONE,
                 PNG_COMPRESSION_TYPE_BASE,
                 PNG_FILTER_TYPE_DEFAULT);
    png_write_info(png, info);

    for (size_t i = 0; i < imageHeight; i++) {
        const auto byteData = static_cast<const unsigned char *>(imageData);
        const png_const_bytep row = byteData + (channel * imageWidth) * i;
        png_write_row(png, row);
    }
    png_write_end(png, nullptr);
    png_destroy_write_struct(&png, &info);
    const uint64_t pngWriteEnd = TimeUtils::GetCurrentMonoMs();
    Logger() << "Writing PNG Usage:" << pngWriteEnd - pngWriteStart << "ms" << std::endl;
}
