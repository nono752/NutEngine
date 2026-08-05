add_executable(sandbox main.cpp)

target_link_libraries(sandbox PRIVATE NutEngine)

add_custom_target(run
    COMMAND sandbox
    DEPENDS sandbox
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)