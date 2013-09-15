cmake_minimum_required(VERSION 2.8)
project( level-editor)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake-modules/")

find_package(SDL2 REQUIRED)

add_executable(level-editor level-editor.c sdl-helpers.c sdl-helpers.h)

target_link_libraries(level-editor ${SDL2_LIBRARY})
include_directories(level-editor ${SDL2_INCLUDE_DIR})

#copy resources from source-dir to binary-dir
file(COPY "res" DESTINATION ${CMAKE_BINARY_DIR}/)
