//
// Created by 1234 on 2025/3/6.
//

#ifndef IOUTILS_H
#define IOUTILS_H
#include <vector>
#include <string>
class IOUtils {
public:
    IOUtils() = default;

    ~IOUtils() = default;

    static std::vector<char> ReadFile(const std::string &path);
};

#endif //IOUTILS_H
