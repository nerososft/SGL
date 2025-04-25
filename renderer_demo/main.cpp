//
// Created by neoyang on 2025/3/6.
//

#include "GraphicsApp.h"
#include "gpu_engine/log/Log.h"

int main(int argc, char *argv[]) {
    Logger() << "Hello World!\n";
    GraphicsApp app;
    app.SetWindowTitle("HPS Blur Demo");
    app.SetWindowPos(20, 20);
    app.SetWindowSize(1024, 768);
    app.Init();
    // app.CaptureFrame("../../renderer_demo/a.png");
    app.Run();
    return 0;
}
