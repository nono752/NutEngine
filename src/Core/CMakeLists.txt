add_library(Core STATIC 
    src/Window.cpp
    src/Renderer.cpp
    src/RendererSDL.cpp
    src/Application.cpp
)

target_include_directories(Core 
    PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/src"
    PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

target_link_libraries(Core PRIVATE SDL3::SDL3)
target_link_libraries(Core PUBLIC Math)