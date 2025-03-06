//
// Created by neoyang on 2025/3/6.
//

#include <iostream>

#include "../effect_engine/EffectEngine.h"
#include "../effect_engine/filters/impl/GrayFilter.h"

int main(int argc, char *argv[]) {
    std::cout << "Hello World!\n";


    EffectEngine effectEngine;
    if (!effectEngine.Init()) {
        std::cout << "Failed to initialize effect_engine" << std::endl;
        return 0;
    }

    const auto filter = std::make_shared<GrayFilter>();
    effectEngine.Process("", "", filter);

    return 0;
}
