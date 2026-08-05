add_library(ECS STATIC 
    src/Registry.cpp
    src/Entity.cpp
)

target_include_directories(ECS 
    PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/src"
    PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include"
)