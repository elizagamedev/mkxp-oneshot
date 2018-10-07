cmake_minimum_required(VERSION 2.8.11)

set(STEAMWORKS_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../steamworks" CACHE PATH "Path to Steamworks folder")
set(GAME_LAUNCH_NAME "oneshot" CACHE STRING "Game launch name")
option(DEBUG "Debug mode" OFF)

set(SOURCES
	steamshim_parent.cpp
)

include_directories(${STEAMWORKS_PATH}/public)

add_definitions(-DGAME_LAUNCH_NAME="${GAME_LAUNCH_NAME}")

IF(DEBUG)
	add_definitions(-DSTEAMSHIM_DEBUG)
ENDIF()

IF(APPLE)
	link_directories(${STEAMWORKS_PATH}/redistributable_bin/osx32)
ELSEIF(WIN32)
	list(APPEND SOURCES resources.rc)
	link_directories(${STEAMWORKS_PATH}/redistributable_bin/win64)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mwindows")
ELSEIF(LINUX)
	link_directories(${STEAMWORKS_PATH}/redistributable_bin/linux64)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -m64")
ENDIF()

add_executable(steamshim
	${SOURCES}
)

IF(WIN32)
	target_link_libraries(steamshim steam_api64)
ELSE()
	target_link_libraries(steamshim steam_api)
ENDIF()
