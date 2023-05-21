cmake_minimum_required(VERSION 3.21)
project(ThetaLinux VERSION 0.0.1)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED true)

set(CMAKE_CXXFLAGS_RELEASE "-Wall -Werror -std=c++20 -O2")

include(FetchContent)
FetchContent_Declare(
 SFML
 GIT_REPOSITORY https://github.com/SFML/SFML.git
 GIT_TAG 2.5.1
)
FetchContent_MakeAvailable(SFML)

set(INCLUDE_DIR /Engine/headers)
include_directories (${INCLUDE_DIR})

file(GLOB_RECURSE ENGINE_SRC  Engine/*.cpp)

add_executable(ThetaLinux ${ENGINE_SRC} main.cpp)
target_link_libraries(ThetaLinux sfml-graphics sfml-window)
target_compile_features(ThetaLinux PRIVATE cxx_std_20)


install(TARGETS ThetaLinux)

