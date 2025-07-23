if (CMAKE_SYSTEM_NAME STREQUAL "Linux")
    message(STATUS "Running on Linux")
elseif (CMAKE_SYSTEM_NAME STREQUAL "Windows")
    message(STATUS "Running on Windows")
elseif (CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    message(STATUS "Running on macOS")
    link_directories("/opt/homebrew/opt/llvm/lib")
    include_directories("/opt/homebrew/opt/llvm/include")
endif ()

set(SLANG_SOURCE main.cpp)
add_executable(slang
        ${SLANG_SOURCE}
)
list(APPEND LLVM_LIBS
        LLVMSupport
        LLVMDemangle
)

list(APPEND MLIR_LIBS
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
        MLIRInferTypeOpInterface
)
target_link_libraries(slang ${LLVM_LIBS} ${MLIR_LIBS})