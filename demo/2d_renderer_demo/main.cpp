#include "Painter.h"
#include "core/log/Log.h"
//
// Created by neo on 25-6-17.
//
int main(int argc, char *argv[]) {
    Logger() << "Hello World!\n";
    // TODO: 2d painter impl for qt

    Painter painter;
    painter.Init(1024,768);

    painter.toImage("../../../demo/2d_renderer_demo/painter.png");
    return 0;
}
