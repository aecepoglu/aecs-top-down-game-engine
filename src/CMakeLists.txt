cmake_minimum_required(VERSION 2.8)
project( aecEngine)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake-modules/")
find_package(SDL2 REQUIRED)
find_package(SDL2_image REQUIRED)
find_package(SDL2_mixer REQUIRED)
find_package(Lua 5.2 REQUIRED)

set(MY_BRUSH_SOURCES brushes rotate terrain object ai)
set(MY_AI_SOURCES leftTurner hungryLeftTurner simpleFlower creeperPlant peekaboo weepingAngel door lineSensor switch)
set(MY_FOV_SOURCES fov-util fov-raycast fov-line)
set(MY_PF_SOURCES roughAStar roughAStar_depthLimited)


foreach(x ${MY_AI_SOURCES})
	list(APPEND _tmp "ai/${x}.c")
endforeach(x ${MY_AI_SOURCES})
set(MY_AI_SOURCES ${_tmp})

unset(_tmp)
foreach(x ${MY_FOV_SOURCES})
	list(APPEND _tmp "fov/${x}.c")
endforeach(x ${MY_FOV_SOURCES})
set(MY_FOV_SOURCES ${_tmp})

unset(_tmp)
foreach(x ${MY_PF_SOURCES})
	list(APPEND _tmp "pf/${x}.c")
endforeach(x ${MY_PF_SOURCES})
set(MY_PF_SOURCES ${_tmp})

unset(_tmp)
foreach(x ${MY_BRUSH_SOURCES})
	list(APPEND _tmp "level-editor/${x}.c")
endforeach(x ${MY_BRUSH_SOURCES})
set(MY_BRUSH_SOURCES ${_tmp})

set(MY_SOURCE_FILES engine.c texture.c map.c map.h object.c vector.c linkedList.c text.c textConsole.c inventory.c cutscene.c)

set(SDLGUI_SOURCE_FILES sdl2gui/sdl2gui.c sdl2gui/sdl2gui-list.c sdl2gui/sdl2gui-text.c sdl2gui/sdl2gui-element.c sdl2gui/sdl2gui-panel.c sdl2gui/sdl2gui-textbox.c)


set(ALL_SOURCE_FILES ${MY_SOURCE_FILES})
list(APPEND ALL_SOURCE_FILES ${MY_AI_SOURCES} ${MY_PF_SOURCES})



add_executable(level-editor level-editor.c ${ALL_SOURCE_FILES} ${SDLGUI_SOURCE_FILES} ${MY_BRUSH_SOURCES} )
target_link_libraries(level-editor ${SDL2_LIBRARY} ${SDL2_IMAGE_LIBRARIES} ${LUA_LIBRARIES})
include_directories(level-editor ${SDL2_INCLUDE_DIR} ${SDL2_IMAGE_INCLUDE_DIRS})

add_executable(aec-game game.c audio.c ${ALL_SOURCE_FILES} ${MY_FOV_SOURCES})
target_link_libraries(aec-game ${SDL2_LIBRARY} ${SDL2_IMAGE_LIBRARIES} ${SDL2_MIXER_LIBRARIES} ${LUA_LIBRARIES})
include_directories(aec-game ${SDL2_INCLUDE_DIR} ${SDL2_IMAGE_INCLUDE_DIRS} ${SDL2_MIXER_INCLUDE_DIRS} ${LUA_INCLUDE_DIR})

target_link_libraries(level-editor m)
target_link_libraries(aec-game m)

add_definitions(-Wall)

set(MY_BUILD_TYPE "release" CACHE STRING "build type; debug or release")
if(MY_BUILD_TYPE STREQUAL "debug")
	add_definitions(-D VERBOSE_3)
	add_definitions(-g)
else(MY_BUILD_TYPE strequal "debug")
	add_definitions(-D VERBOSE_0)
endif(MY_BUILD_TYPE STREQUAL "debug")

#copy resources from source-dir to binary-dir
file(COPY "res" DESTINATION ${CMAKE_BINARY_DIR}/)
