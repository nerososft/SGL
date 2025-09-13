#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Weverything"
#pragma clang diagnostic warning "-Wextra"
#pragma clang diagnostic warning "-Wall"
#include "sgl_image.napi.h"
EXTERN_C_START
napi_value Init(napi_env env, napi_value exports) {
    Init__sgl_image(env, exports);
    return exports;
}
EXTERN_C_END
static napi_module demoModule = {
    .nm_version = 1,
    .nm_flags = 0,
    .nm_filename = nullptr,
    .nm_register_func = Init,
    .nm_modname = "entry",
    .nm_priv = ((void*)0),
    .reserved = { 0 },
};
extern "C" __attribute__((constructor)) void RegisterEntryModule(void)
{
    napi_module_register(&demoModule);
}
#pragma clang diagnostic pop
