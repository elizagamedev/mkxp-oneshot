cmake_minimum_required(VERSION 2.8.11)
Project(oneshot)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup(TARGETS)

## Setup options ##

option(STEAM "Build for Steam" OFF)
set(STEAMWORKS_PATH "${CMAKE_CURRENT_SOURCE_DIR}/steamworks" CACHE PATH "Path to Steamworks folder")
option(DEBUG "Debug mode" OFF)

## Misc setup ##

set(CMAKE_INCLUDE_CURRENT_DIR ON)

## Setup main source ##

set(MAIN_HEADERS
	src/quadarray.h
	src/audio.h
	src/binding.h
	src/bitmap.h
	src/disposable.h
	src/etc.h
	src/etc-internal.h
	src/eventthread.h
	src/flashable.h
	src/font.h
	src/input.h
	src/plane.h
	src/scene.h
	src/sprite.h
	src/table.h
	src/texpool.h
	src/tilequad.h
	src/transform.h
	src/viewport.h
	src/window.h
	src/serializable.h
	src/shader.h
	src/glstate.h
	src/quad.h
	src/tilemap.h
	src/tilemap-common.h
	src/graphics.h
	src/gl-debug.h
	src/global-ibo.h
	src/exception.h
	src/filesystem.h
	src/serial-util.h
	src/intrulist.h
	src/binding.h
	src/gl-util.h
	src/util.h
	src/config.h
	src/settingsmenu.h
	src/keybindings.h
	src/tileatlas.h
	src/sharedstate.h
	src/al-util.h
	src/boost-hash.h
	src/debugwriter.h
	src/gl-fun.h
	src/gl-meta.h
	src/vertex.h
	src/soundemitter.h
	src/aldatasource.h
	src/alstream.h
	src/audiostream.h
	src/rgssad.h
	src/sdl-util.h
	src/oneshot.h
	src/pipe.h
	chromasdk/RzChromaSDKDefines.h
	chromasdk/RzChromaSDKTypes.h
	chromasdk/RzErrors.h
	chromasdk/ChromaApi.h
	src/i18n.h
)

set(MAIN_SOURCE
	src/main.cpp
	src/audio.cpp
	src/bitmap.cpp
	src/eventthread.cpp
	src/filesystem.cpp
	src/font.cpp
	src/input.cpp
	src/plane.cpp
	src/scene.cpp
	src/sprite.cpp
	src/table.cpp
	src/tilequad.cpp
	src/viewport.cpp
	src/window.cpp
	src/texpool.cpp
	src/shader.cpp
	src/glstate.cpp
	src/tilemap.cpp
	src/autotiles.cpp
	src/graphics.cpp
	src/gl-debug.cpp
	src/etc.cpp
	src/config.cpp
	src/settingsmenu.cpp
	src/keybindings.cpp
	src/tileatlas.cpp
	src/sharedstate.cpp
	src/gl-fun.cpp
	src/gl-meta.cpp
	src/vertex.cpp
	src/soundemitter.cpp
	src/sdlsoundsource.cpp
	src/alstream.cpp
	src/audiostream.cpp
	src/rgssad.cpp
	src/vorbissource.cpp
	src/oneshot.cpp
	src/screen.cpp
	src/i18n.cpp

)

if(WIN32)
	list(APPEND MAIN_SOURCE assets/resources.rc)
	list(APPEND DEFINES UNICODE)
	list(APPEND PLATFORM_LIBRARIES Secur32 Shlwapi)
	include_directories(
		${CMAKE_CURRENT_BINARY_DIR}/windows
	)
elseif(APPLE)
	list(APPEND MAIN_HEADERS src/mac-desktop.h)
	list(APPEND MAIN_SOURCE src/mac-desktop.mm)
else()
	find_package(PkgConfig REQUIRED)
	pkg_check_modules(LINUXPKGS REQUIRED gtk+-2.0 libxfconf-0)
	include_directories(${LINUXPKGS_INCLUDE_DIRS})
	add_compile_options(${LINUXPKGS_CFLAGS_OTHER})
	list(APPEND PLATFORM_LIBRARIES ${LINUXPKGS_LDFLAGS})
	list(APPEND MAIN_SOURCE src/xdg-user-dir-lookup.c)
	list(APPEND MAIN_HEADERS src/xdg-user-dir-lookup.h)
endif()

if (STEAM)
	list(APPEND MAIN_HEADERS
		src/steam.h
		steamshim/steamshim_child.h
	)
	list(APPEND MAIN_SOURCE
		src/steam.cpp
		steamshim/steamshim_child.c
	)
	list(APPEND DEFINES
		STEAM
	)
endif()

source_group("MKXP Source" FILES ${MAIN_SOURCE} ${MAIN_HEADERS})

## Setup embedded source ##

set(EMBEDDED_INPUT
	shader/common.h
	shader/transSimple.frag
	shader/trans.frag
	shader/hue.frag
	shader/sprite.frag
	shader/plane.frag
	shader/gray.frag
	shader/bitmapBlit.frag
	shader/flatColor.frag
	shader/simple.frag
	shader/simpleColor.frag
	shader/simpleAlpha.frag
	shader/simpleAlphaUni.frag
	shader/flashMap.frag
	shader/obscured.frag
	shader/minimal.vert
	shader/simple.vert
	shader/simpleColor.vert
	shader/sprite.vert
	shader/tilemap.vert
	shader/blur.frag
	shader/blurH.vert
	shader/blurV.vert
	shader/simpleMatrix.vert
	assets/icon.png
	assets/gamecontrollerdb.txt
)

if (RGSS2)
	list(APPEND DEFINES
		RGSS2
	)
endif()

if (MSVC)
	list(APPEND DEFINES
		_CRT_SECURE_NO_WARNINGS
		NOMINMAX
	)
endif()

## Process Embeddeds ##

find_program(XXD_EXE xxd
	DOC "Location of the xxd executable"
)

macro(ProcessWithXXD outvar inputfile outdir)
	get_filename_component(basefile ${inputfile} NAME)
	set(outputfile ${outdir}/${basefile}.xxd)
	set_source_files_properties(${outputfile} PROPERTIES HEADER_ONLY TRUE)
	add_custom_command(
		OUTPUT ${outputfile}
		COMMAND ${XXD_EXE} -i ${inputfile} ${outputfile}
		WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
		DEPENDS ${inputfile}
		COMMENT "Generating XXD for ${inputfile}"
	)
	list(APPEND ${outvar}
		${outputfile}
	)
endmacro()

file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/xxdhack)
include_directories(
	${CMAKE_CURRENT_BINARY_DIR}/xxdhack
)

foreach(item ${EMBEDDED_INPUT})
	ProcessWithXXD(EMBEDDED_SOURCE ${item} ${CMAKE_CURRENT_BINARY_DIR})
endforeach()

source_group("Embedded Source" FILES ${EMBEDDED_INPUT} ${EMBEDDED_SOURCE})

## Setup binding source ##

list(APPEND DEFINES
	BINDING_MRI
)
set(BINDING_HEADERS
	binding-mri/binding-util.h
	binding-mri/binding-types.h
	binding-mri/serializable-binding.h
	binding-mri/disposable-binding.h
	binding-mri/sceneelement-binding.h
	binding-mri/viewportelement-binding.h
	binding-mri/flashable-binding.h
)
set(BINDING_SOURCE
	binding-mri/binding-mri.cpp
	binding-mri/binding-util.cpp
	binding-mri/table-binding.cpp
	binding-mri/etc-binding.cpp
	binding-mri/bitmap-binding.cpp
	binding-mri/font-binding.cpp
	binding-mri/graphics-binding.cpp
	binding-mri/input-binding.cpp
	binding-mri/sprite-binding.cpp
	binding-mri/viewport-binding.cpp
	binding-mri/plane-binding.cpp
	binding-mri/window-binding.cpp
	binding-mri/tilemap-binding.cpp
	binding-mri/audio-binding.cpp
	binding-mri/module_rpg.cpp
	binding-mri/filesystem-binding.cpp
	binding-mri/oneshot-binding.cpp
	binding-mri/steam-binding.cpp
	binding-mri/wallpaper-binding.cpp
	binding-mri/journal-binding.cpp
	binding-mri/chroma-binding.cpp
	binding-mri/niko-binding.cpp
	binding-mri/modshot-window-binding.cpp
	binding-mri/modshot-fiber-binding.cpp
)

source_group("Binding Source" FILES ${BINDING_SOURCE} ${BINDING_HEADERS})

## Setup main executable ##

set(CMAKE_CXX_STANDARD 11)

add_executable(${PROJECT_NAME} MACOSX_BUNDLE WIN32
	${MAIN_HEADERS}
	${MAIN_SOURCE}
	${BINDING_HEADERS}
	${BINDING_SOURCE}
	${EMBEDDED_SOURCE}
)

target_compile_definitions(${PROJECT_NAME} PRIVATE ${DEFINES})
target_include_directories(${PROJECT_NAME} PRIVATE src)
target_link_libraries(${PROJECT_NAME}
	CONAN_PKG::boost
	CONAN_PKG::openal
	CONAN_PKG::physfs
	CONAN_PKG::pixman
	CONAN_PKG::ruby
	CONAN_PKG::sdl2
	CONAN_PKG::sdl2_image
	CONAN_PKG::sdl2_ttf
	CONAN_PKG::sdl_sound-mkxp
	CONAN_PKG::sigc++
	${PLATFORM_LIBRARIES})

IF(APPLE)
	add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD COMMAND cmake -P "${CMAKE_SOURCE_DIR}/patches/mac/CompleteBundle.cmake" VERBATIM)
ENDIF()

if(STEAM)
	add_subdirectory(steamshim_parent)

	# steam_appid.txt
	configure_file(
		"${CMAKE_SOURCE_DIR}/steam_appid.txt"
		"${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/steam_appid.txt"
		COPYONLY)
endif()

#add_subdirectory(scripts)
