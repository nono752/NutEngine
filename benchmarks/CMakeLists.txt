find_package(benchmark CONFIG REQUIRED)

add_executable(ECS_Benchmarks 
    ECS/bench_View.cpp
    ECS/bench_Registry.cpp
    ECS/bench_Entity.cpp
    ECS/bench_SparseSet.cpp
)

target_link_libraries(ECS_Benchmarks PRIVATE 
    NutEngine 
    benchmark::benchmark 
    benchmark::benchmark_main
)