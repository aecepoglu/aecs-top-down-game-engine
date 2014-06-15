cmake_minimum_required(VERSION 2.8)
project( aecEngine)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake-modules/")
find_package(SDL2 REQUIRED)
find_package(SDL2_image REQUIRED)

#list ai names here. They'll be prepended by "ai/" and appended by ".c" and ".h"
set(MY_AI_SOURCES leftTurner hungryLeftTurner simpleFlower creeperPlant peekaboo)

foreach(x ${MY_AI_SOURCES})
	list(APPEND _tmp "ai/${x}.c" "ai/${x}.h")
endforeach(x ${MY_AI_SOURCES})
set(MY_AI_SOURCES ${_tmp})


set(MY_SOURCE_FILES engine.c engine.h texture.c texture.h map.c map.h object.c object.h brush.h vector.c vector.h basic.h array.h linkedList.h linkedList.c log.h ai.h aiFun.h aiTable.h pf/roughAStar.c pf/roughAStar.h )


set(ALL_SOURCE_FILES ${MY_SOURCE_FILES})
list(APPEND ALL_SOURCE_FILES ${MY_AI_SOURCES})



add_executable(levelEditor level-editor.c ${ALL_SOURCE_FILES})
target_link_libraries(levelEditor ${SDL2_LIBRARY} ${SDL2_IMAGE_LIBRARIES})
include_directories(levelEditor ${SDL2_INCLUDE_DIR} ${SDL2_IMAGE_INCLUDE_DIRS})

add_executable(aec-game game.c ${ALL_SOURCE_FILES})
target_link_libraries(aec-game ${SDL2_LIBRARY} ${SDL2_IMAGE_LIBRARIES})
include_directories(aec-game ${SDL2_INCLUDE_DIR} ${SDL2_IMAGE_INCLUDE_DIRS})

add_definitions(-Wall)

#copy resources from source-dir to binary-dir
file(COPY "res" DESTINATION ${CMAKE_BINARY_DIR}/)
file(COPY "test-maps" DESTINATION ${CMAKE_BINARY_DIR}/)
