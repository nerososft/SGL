add_executable(inference_demo
        main.cpp
        Tokenizer.cpp
        Tokenizer.h
        SafeTensor.cpp
        SafeTensor.h
        Model.cpp
        Model.h
        TransformerBlock.cpp
        TransformerBlock.h
        Config.cpp
        Config.h
        Infer.cpp
        Infer.h
)
target_link_libraries(inference_demo compute_engine)