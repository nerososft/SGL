//
// Created by neo on 2025/3/17.
//

#ifndef CONFIG_H
#define CONFIG_H

#define VERSION "1.0.9"

#ifdef OS_OPEN_HARMONY
#define SHADER_PATH "/data/storage/el1/bundle/entry/resources/resfile/shader_compiled/"
#else

#endif




#define SHADER(path) (SHADER_PATH # path)

#endif //CONFIG_H
