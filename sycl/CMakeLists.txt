if (CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    add_subdirectory(compiler) # Testing
    add_subdirectory(examples)
endif ()