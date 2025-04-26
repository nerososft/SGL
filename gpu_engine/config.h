//
// Created by neo on 2025/3/17.
//

#ifndef CONFIG_H
#define CONFIG_H

#define VERSION "1.0.9"

#ifdef OS_OPEN_HARMONY
#define SHADER_PATH "/data/storage/el1/bundle/entry/resources/resfile/shader_compiled/"
#else
#ifdef __APPLE__
#define SHADER_PATH "../../shader_compiled/"
#elif defined(__linux__)
#define SHADER_PATH "../../shader_compiled/"
#else
#define SHADER_PATH "../../pseffect/shader_compiled/"
#endif /* __APPLE__ */
#endif


#ifdef __APPLE__
#define LOG_TO_FILE 0
#elif defined(__linux__)
#define LOG_TO_FILE 0
#else
#define LOG_TO_FILE 1
#endif


#define LOG_FILE_PATH "d://temp//output.txt"

#define SHADER(path) (SHADER_PATH # path)

#endif //CONFIG_H
