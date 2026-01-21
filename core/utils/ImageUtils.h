//
// Created by neo on 2025/3/10.
//

#ifndef IMAGEUTILS_H
#define IMAGEUTILS_H
#include <cstdint>
#include <string>
#include <vector>

class ImageUtils {
public:
  static void ReadPngFileSize(const std::string &fileName, uint32_t *imageWidth,
                              uint32_t *imageHeight, uint32_t *channel);

  static void *ReadPngFileRow(const std::string &fileName, uint32_t row);

  static std::vector<char> ReadPngFile(const std::string &fileName,
                                       uint32_t *imageWidth,
                                       uint32_t *imageHeight,
                                       uint32_t *channel);

  static std::vector<char> ReadJpgFile(const std::string &fileName,
                                       uint32_t *imageWidth,
                                       uint32_t *imageHeight,
                                       uint32_t *channel);

  static void WritePngFile(const std::string &fileName, uint32_t imageWidth,
                           uint32_t imageHeight, uint32_t channel,
                           const void *imageData);
};

#endif // IMAGEUTILS_H
