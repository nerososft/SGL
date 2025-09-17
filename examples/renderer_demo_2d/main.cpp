//
// Created by neoyang on 2025/3/6.
//

#include "GraphicsApp.h"
#include "runtime/log/Log.h"

int main(int argc, char *argv[]) {
  Logger() << "Hello World!\n";

  GraphicsApp app;
  app.SetWindowTitle("2D Render Demo");
  app.SetWindowPos(0, 0);
  // app.SetWindowMode(WINDOW_MODE_FULLSCREEN);
  app.SetWindowSize(3840, 2556);
  if (!app.Init()) {
    Logger() << Logger::ERROR << "Failed to initialize" << std::endl;
    return 1;
  }
  app.CaptureFrame("../../../examples/renderer_demo_2d/render_offscreen.png");
  app.Run();
  return 0;
}
