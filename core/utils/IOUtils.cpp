//
// Created by 1234 on 2025/3/6.
//

#include "IOUtils.h"

#include <fstream>
#include <iostream>

#include "log/Log.h"

std::vector<char> IOUtils::ReadFile(const std::string &path) {
    std::vector<char> data;
    std::ifstream file;
    file.open(path.c_str(), std::ios::binary | std::ios::in | std::ios::ate);
    if (file.fail()) {
        Logger() << "Failed to open file: " << path << std::endl;
        return data;
    }
    const size_t filesize = file.tellg();
    data.resize(filesize);
    file.seekg(0);
    file.read(data.data(), filesize);
    file.close();
    return data;
}
