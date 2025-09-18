#pragma once
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Weverything"
#pragma clang diagnostic warning "-Wextra"
#pragma clang diagnostic warning "-Wall"
#include "taihe/napi_runtime.hpp"
#if __has_include(<node/node_api.h>)
#include <node/node_api.h>
#elif __has_include(<napi/native_api.h>)
#include <napi/native_api.h>
#else
#error "Please ensure the napi is correctly installed."
#endif
#ifndef __sgl_image_NAPI_H
#define __sgl_image_NAPI_H
EXTERN_C_START
napi_value Init__sgl_image(napi_env env, napi_value exports);
EXTERN_C_END
#endif
#pragma clang diagnostic pop
