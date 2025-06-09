//
// Created by neo on 25-5-3.
//

#ifndef RENDERERSCENE_H
#define RENDERERSCENE_H
#include <memory>
#include <vector>

#include "RendererCamera.h"
#include "RendererLight.h"
#include "RendererMesh.h"


class RendererScene {
    std::vector<std::shared_ptr<RendererLight> > lights;
    std::shared_ptr<RendererCamera> camera = nullptr;
    std::vector<std::shared_ptr<RendererMesh> > meshes;

public:
    RendererScene() = default;

    ~RendererScene() = default;
};


#endif //RENDERERSCENE_H
