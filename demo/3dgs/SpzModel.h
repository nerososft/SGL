//
// Created by neo on 25-6-12.
//

#ifndef SPZFILELOADER_H
#define SPZFILELOADER_H


#include <cstdint>

#include "GaussianSplatting3DRenderer.h"
#include "GuassianModel.h"
#include "core/utils/IOUtils.h"

#define SPZ_HEADER_MAGIC (0x5053474e)

struct SpzFileHeader {
    uint32_t magic;
    uint32_t version;
    uint32_t numPoints;
    uint8_t shDegree;
    uint8_t fractionalBits;
    uint8_t flags;
    uint8_t reserved;
};

struct SpzFilePosition {
    uint8_t x[3];
    uint8_t y[3];
    uint8_t z[3];
} __attribute__((packed));

struct SpzFileScale {
    uint8_t x;
    uint8_t y;
    uint8_t z;
} __attribute__((packed));

struct SpzFileRotation {
    uint8_t x;
    uint8_t y;
    uint8_t z;
} __attribute__((packed));

struct SpzFileAlpha {
    uint8_t x;
} __attribute__((packed));

struct SpzFileColor {
    uint8_t x;
    uint8_t y;
    uint8_t z;
} __attribute__((packed));

struct SpzFileSphericalHarmonic9 {
    uint8_t sh1n1_r;
    uint8_t sh1n1_g;
    uint8_t sh1n1_b;
    uint8_t sh10_r;
    uint8_t sh10_g;
    uint8_t sh10_b;
    uint8_t sh1p1_r;
    uint8_t sh1p1_g;
    uint8_t sh1p1_b;
};

struct SpzFileSphericalHarmonic24 {
    // TODO:
};

struct SpzFileSphericalHarmonic45 {
    // TODO:
};

struct SpzFile {
    SpzFileHeader *header;
    SpzFilePosition *positions;
    SpzFileScale *scales;
    SpzFileRotation *rotations;
    SpzFileAlpha *alphas;
    SpzFileColor *colors;

    union {
        SpzFileSphericalHarmonic9 *sphericalHarmonics;
    };
};

class SpzModel {
    std::vector<char> modelBytes;
    std::vector<GaussianPoint> gaussianPoints;

public:
    bool loadModel(const char *str);

    std::vector<GaussianPoint> getPoints() {
        return gaussianPoints;
    };
};

#endif //SPZFILELOADER_H
