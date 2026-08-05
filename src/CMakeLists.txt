add_subdirectory(Core)
add_subdirectory(ECS)
add_subdirectory(Math)

add_library(NutEngine INTERFACE)

target_link_libraries(NutEngine INTERFACE 
    Core 
    ECS
    Math
)