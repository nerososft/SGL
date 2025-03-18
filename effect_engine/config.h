//
// Created by neo on 2025/3/17.
//

#ifndef CONFIG_H
#define CONFIG_H

#define VERSION "1.0.6"

#ifdef OS_OPEN_HARMONY
#define SHADER_PATH "/data/storage/el1/bundle/entry/resources/resfile/shader_compiled/"
#else
#define SHADER_PATH "../../shader_compiled/"
#endif
#define SHADER_PATH "/data/storage/el2/base/haps/entry/files/AppData/himirage/"

#define SHADER(path) (SHADER_PATH # path)

#endif //CONFIG_H
