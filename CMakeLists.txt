cmake_minimum_required(VERSION 2.8)
project( levelEditor)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake-modules/")

find_package(SDL2 REQUIRED)

add_executable(levelEditor level-editor.c texture.c texture.h map.c map.h basic.c basic.h brush.h)

target_link_libraries(levelEditor ${SDL2_LIBRARY})
include_directories(levelEditor ${SDL2_INCLUDE_DIR})

#copy resources from source-dir to binary-dir
file(COPY "res" DESTINATION ${CMAKE_BINARY_DIR}/)
