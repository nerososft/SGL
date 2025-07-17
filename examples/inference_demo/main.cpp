//
// Created by neo on 25-5-7.
//
#include <iostream>

#include "Infer.h"
#include "core/log/Log.h"

int main(int argc, char* argv[])
{
    std::cout << "Inference demo" << std::endl;

    const std::string prompt = "<|im_start|>user"
        "Give me a short introduction to large language model.<|im_end|>"
        "<|im_start|>assistant";

    Infer infer;
    infer.Init();
    infer.Run(prompt);

    return 0;
}
