if (CMAKE_SYSTEM_NAME STREQUAL "Linux")
    message(STATUS "Running on Linux")
elseif (CMAKE_SYSTEM_NAME STREQUAL "Windows")
    message(STATUS "Running on Windows")
elseif (CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    message(STATUS "Running on macOS")
    if (SGLANG_USE_MLIR)
        message("SGLANG_USE_MLIR is defined, use MLIR")
        link_directories("/opt/homebrew/opt/llvm/lib")
        include_directories("/opt/homebrew/opt/llvm/include")
    else ()
        message("SGLANG_USE_MLIR is not defined, use GLSLBuilder")
        link_directories("/opt/homebrew/Cellar/shaderc/2025.3/lib")
        include_directories("/opt/homebrew/Cellar/shaderc/2025.3/include")
    endif ()
endif ()

set(SLANG_SOURCE
        compiler/ShaderCompiler.cpp
        compiler/ShaderCompiler.h
        main.cpp)
add_executable(slang
        ${SLANG_SOURCE}
)
if (SGLANG_USE_MLIR)
    list(APPEND LLVM_LIBS
            LLVMSupport
            LLVMDemangle
    )

    list(APPEND MLIR_LIBS
            MLIR
            MLIRSupport
            MLIRIR
            MLIRPass
            MLIRSPIRVAttrToLLVMConversion
            MLIRSPIRVBinaryUtils
            MLIRSPIRVConversion
            MLIRSPIRVDeserialization
            MLIRSPIRVDialect
            MLIRSPIRVModuleCombiner
            MLIRSPIRVSerialization
            MLIRTransforms
            MLIRFunctionInterfaces
            MLIRFuncToSPIRV
            MLIRControlFlowToSPIRV
            MLIRControlFlowDialect
            MLIRControlFlowInterfaces
            MLIRUBToSPIRV
            MLIRUBDialect
            MLIRCallInterfaces
    )
    target_link_libraries(slang ${LLVM_LIBS} ${MLIR_LIBS})
else ()
    list(APPEND SHADER_COMPILER_LIBS
            shaderc_combined
    )
    target_link_libraries(slang ${SHADER_COMPILER_LIBS})
endif ()