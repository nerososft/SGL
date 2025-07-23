//
// Created by neo on 25-7-23.
//

#include "sgl.h"

#include <cstdlib>

sgl_t* sgl_create() {
    const auto sgl = static_cast<sgl_t*>(malloc(sizeof(sgl_t)));
    if (sgl == nullptr) {
        return nullptr;
    }
    // TODO:
    return nullptr;
}

void sgl_destroy(sgl_t* sgl) {
    if (sgl == nullptr) {
        return;
    }
    free(sgl);
}
