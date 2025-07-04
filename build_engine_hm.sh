cloc . --exclude-dir=cmake-build-debug,models,vendor,.git,.venv,.idea

rm -rf build_os
mkdir build_os
cd build_os

cmake .. -DCMAKE_BUILD_TYPE:STRING=Release \
-DCMAKE_CXX_COMPILER:FILEPATH=D:/OHOS/DevEcoStudio/sdk/default/openharmony/native/llvm/bin/clang.exe \
-DCMAKE_C_COMPILER:FILEPATH=D:/OHOS/DevEcoStudio/sdk/default/openharmony/native/llvm/bin/clang++.exe \
-DCMAKE_CXX_FLAGS_INIT:STRING=-DQT_QML_DEBUG \
-DCMAKE_MAKE_PROGRAM:FILEPATH=D:/mingw_qt/mingw32/bin/mingw32-make.exe \
-DCMAKE_PREFIX_PATH:PATH=D:/qtReInstall/5.15.2/ \
-DCMAKE_TOOLCHAIN_FILE:FILEPATH=D:/OHOS/DevEcoStudio/sdk/default/openharmony/native/build/cmake/ohos.toolchain.cmake \
-DOHOS_ARCH:STRING=arm64-v8a \
-DOHOS_PLATFORM:STRING=OHOS \
-DCMAKE_GENERATOR:STRING="MinGW Makefiles"\

D:/mingw_qt/mingw32/bin/mingw32-make.exe -j32

cp /d/develop_main/main_v4/pseffect/build_os/effect_engine/libgpu_effect_engine.so  /d/develop_main/main_v4/photosir_oh/entry/libs/arm64-v8a
cp /d/develop_main/main_v4/pseffect/build_os/effect_engine/libgpu_effect_engine.so  /d/develop_main/main_v4/lib/harmonyos/Release


