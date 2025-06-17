//
// Created by neoyang on 2025/3/6.
//

#include "GraphicsApp.h"
#include "core/log/Log.h"

int main(int argc, char *argv[]) {
    Logger() << "Hello World!\n";

    GraphicsApp app;
    app.SetWindowTitle("Renderer Demo");
    app.SetWindowPos(20, 20);
    app.SetWindowSize(768, 768);
    app.Init();
    app.CaptureFrame("../../../demos/3d_renderer_demo/render_offscreen.png");
    app.Run();
    return 0;
}
