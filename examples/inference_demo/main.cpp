//
// Created by neo on 25-5-7.
//
#include <iostream>

#include "Infer.h"
#include "core/log/Log.h"

int main(int argc, char* argv[])
{
    std::cout << "Inference demo" << std::endl;

    const std::string prompt = "{\"role\": \"user\", \"content\": \"Give me a short introduction to large language model.\"}";

    Infer infer;
    infer.Init();
    infer.Run(prompt);

    return 0;
}
