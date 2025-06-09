//
// Created by neo on 2025/3/17.
//

#ifndef LOG_H
#define LOG_H
#include <string>

#include <iostream>
#include <sstream>
#include <fstream>
#include "core/config.h"
#ifdef OS_OPEN_HARMONY
#include <hilog/log.h>
#endif

static std::ofstream g_log_file;

class Logger {
public:
    enum Level { DEBUG, INFO, WARNING, ERROR };

    std::ostringstream *buffer = nullptr;
    std::streambuf *stdoutBuffer = std::cout.rdbuf();

    explicit Logger(const Level lvl = INFO) : level(lvl) {
        buffer = new std::ostringstream;
        if constexpr (LOG_TO_FILE) {
            g_log_file.open(LOG_FILE_PATH, std::ofstream::out | std::ofstream::app);
            if (!g_log_file.is_open()) {
                std::cerr << "Failed to open log file!" << std::endl;
                return;
            }
            std::cout.rdbuf(g_log_file.rdbuf());
        }
    }

    ~Logger() {
        output();
        std::cout.rdbuf(stdoutBuffer);
        delete buffer;
        g_log_file.close();
    }

    template<typename T>
    Logger &operator<<(const T &msg) {
        if (buffer) *buffer << msg;
        return *this;
    }

    Logger &operator<<(std::ostream & (*manip)(std::ostream &)) {
        if (buffer) manip(*buffer);
        return *this;
    }

    Logger &operator<<(const Level lvl) {
        output();
        level = lvl;
        return *this;
    }

private:
    Level level;

    void output() const {
        if (buffer == nullptr) return;
        if (buffer->tellp() > 0) {
#ifdef OS_OPEN_HARMONY
            OH_LOG_Print(LOG_APP, LOG_INFO, 0xFF00, "[EffectEngine]", "%{public}s", buffer->str().c_str());
#else
            std::cout << getLevelStr() << buffer->str();
#endif /* OS_OPEN_HARMONY */
            buffer->str("");
        }
    }

    [[nodiscard]] const char *getLevelStr() const {
        static const char *levels[] = {"[DEBUG] ", "[INFO] ", "[WARN] ", "[ERROR] "};
        return levels[level];
    }
};

#endif //LOG_H
