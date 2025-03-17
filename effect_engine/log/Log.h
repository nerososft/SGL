//
// Created by neo on 2025/3/17.
//

#ifndef LOG_H
#define LOG_H
#include <string>

#include <iostream>
#include <sstream>
#ifdef Q_OS_OPENHARMONY
#include <hilog/log.h>
#endif

class Logger {
public:
    enum Level { DEBUG, INFO, WARNING, ERROR };

    explicit Logger(const Level lvl = INFO) : buffer(new std::ostringstream), level(lvl) {
    }

    ~Logger() {
        if (buffer) {
            output();
            delete buffer;
        }
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
    std::ostringstream *buffer;
    Level level;

    void output() const {
        if (buffer->tellp() > 0) {
#ifdef OS_OPEN_HARMONY
            OH_LOG_Print(LOG_APP, LOG_INFO, 0xFF00, "[EffectEngine]", buffer.str());
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
