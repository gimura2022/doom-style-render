cmake_minimum_required(VERSION 3.5)
project(HaltyraEngine C ASM)

add_compile_options(-g)
add_compile_options(-fPIE)
add_compile_options(-Wall)
add_compile_options(-Wextra)

enable_testing()

if (WIN32)
    set(SDL2TTF_DIR ./SDL2_ttf/)
    find_package(SDL2TTF REQUIRED)
else()
    find_package(SDL2_TTF REQUIRED)
endif()

find_package(SDL2 REQUIRED)

include_directories(${SDL2_INCLUDE_DIRS})
include_directories(${SDL_TTF_INCLUDE_DIRS})
include_directories(./src)

add_subdirectory(src    ./src)
add_subdirectory(editor ./editor)
#add_subdirectory(tests  ./tests)
