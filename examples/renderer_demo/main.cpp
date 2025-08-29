//
// Created by neoyang on 2025/3/6.
//

#include "GraphicsApp.h"
#include "runtime/log/Log.h"

int main(int argc, char *argv[]) {
  Logger() << "Hello World!\n";

  GraphicsApp app;
  app.SetWindowTitle("Dual Eye XR Demo");
  app.SetWindowPos(0, 0);
  // app.SetWindowMode(WINDOW_MODE_FULLSCREEN);
  app.SetWindowSize(1920, 1080);
  app.Init();
  app.CaptureFrame("../../../examples/renderer_demo/render_offscreen.png");
  app.Run();
  return 0;
}
