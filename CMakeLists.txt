cmake_minimum_required(VERSION 3.9)

project(NUTENGINE LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(SDL3 CONFIG REQUIRED)

#add_subdirectory(src)
#add_subdirectory(sandbox)
#
## generate targets for testings
#add_subdirectory(tests)
#
## generate targets for benchmarks
#add_subdirectory(benchmarks)
#
## generate targets for documentation
#add_subdirectory(doc)