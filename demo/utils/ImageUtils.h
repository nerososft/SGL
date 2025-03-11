//
// Created by neo on 2025/3/10.
//

#ifndef IMAGEUTILS_H
#define IMAGEUTILS_H
#include <vector>
#include <string>

class ImageUtils {
public:
    static std::vector<char> ReadPngFile(const std::string &fileName,
                                         uint32_t *imageWidth,
                                         uint32_t *imageHeight,
                                         uint32_t *channel);

    static void WritePngFile(const std::string &fileName,
                             uint32_t imageWidth,
                             uint32_t imageHeight,
                             uint32_t channel,
                             const void *imageData);
};


#endif //IMAGEUTILS_H
