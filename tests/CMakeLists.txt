enable_testing()

find_package(GTest CONFIG REQUIRED)
include(GoogleTest)

add_executable(UnitTests
    ECS/test_SparseSet.cpp
    ECS/test_Registry.cpp
    ECS/test_View.cpp
    ECS/test_Entity.cpp
)

target_link_libraries(UnitTests PRIVATE 
    NutEngine
    GTest::gtest
    GTest::gtest_main
)

gtest_discover_tests(UnitTests)