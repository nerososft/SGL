//
// Created by neo on 25-4-30.
//


#include <memory>

#include "gpu_engine/log/Log.h"
#include "renderer/Renderer.h"
#include "scene/ModelLoader.h"

int main(int argc, char *argv[]) {
    Logger() << "Hello World!\n";

    const auto renderer = std::make_shared<Renderer>(768, 768);

    renderer->SetOnLoadScene([](Renderer *rdr) -> bool {
            // TODO：The rendered scene should be passed in by the engine rather than read by the renderer
            // const std::vector<std::shared_ptr<Mesh> > models = ModelLoader::LoadModel(
            //     "../../renderer_demo/assets/builtin.models/Lion.OBJ");
            const std::vector<std::shared_ptr<Mesh> > models = ModelLoader::LoadModel(
                "../../renderer_demo/assets/builtin.models/1911.FBX");

            for (auto &mesh: models) {
                if (!rdr->AddDrawElement(mesh->vertexData, mesh->indicesData, mesh->material, mesh->transform)) {
                    Logger() << "Vertex buffer add failed" << std::endl;
                    return false;
                }
            }
            return true;
        }
    );

    const std::vector<const char *> requiredExtensions;
    if (!renderer->Init(requiredExtensions)) {
        Logger() << "Failed to initialize renderer" << std::endl;
    }
    renderer->RenderFrameOffScreen("../../renderer_demo/images/render_offscreen1.png");
    return 0;
}
