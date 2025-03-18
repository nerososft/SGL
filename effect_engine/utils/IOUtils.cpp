//
// Created by 1234 on 2025/3/6.
//

#include "IOUtils.h"

#include <fstream>
#include <iostream>

#define LOG_TAG "Engine"


#define LOG_DOMAIN 0






std::vector<char> IOUtils::ReadFile(const std::string &path) {
    std::vector<char> data;
    std::ifstream file;
    file.open(path.c_str(), std::ios::binary | std::ios::in | std::ios::ate);
    if (file.fail()) {
        std::cout << "Failed to open file: " << path << std::endl;
#ifdef ENABLE_HIMI_HILOG
        OH_LOG_INFO(LOG_APP, "hwwtest  Failed to open file in util ");
#endif // ENABLE_HIMI_HILOG

        return data;
    }
    const size_t filesize = file.tellg();
    data.resize(filesize);
    file.seekg(0);
    file.read(data.data(), filesize);
    file.close();
    return data;
}
