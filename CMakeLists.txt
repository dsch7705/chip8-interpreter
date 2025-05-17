cmake_minimum_required(VERSION 3.10)
project(chip8-interpreter C)

include(FetchContent)
FetchContent_Declare(
    raylib
    GIT_REPOSITORY https://github.com/raysan5/raylib.git
    GIT_TAG master
)
FetchContent_MakeAvailable(raylib)

add_executable(chip src/main.c src/gui.c src/chip8.c)
target_link_libraries(chip PRIVATE raylib)
target_include_directories(chip PRIVATE include)