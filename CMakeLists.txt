cmake_minimum_required(VERSION 2.8.11)
Project(oneshot)

#set(CONAN_DISABLE_CHECK_COMPILER ON) 

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup(TARGETS)

## Setup options ##

# Note this option only adds steamshim_child into oneshot.
# It's outside this repo's responsibility to compile the parent.
# Contact rkevin for details.
option(STEAM "Build for Steam" OFF)
option(DEBUG "Debug mode" OFF)

## Misc setup ##

set(CMAKE_INCLUDE_CURRENT_DIR ON)

## Setup main source ##

set(SRC_AUDIO_HEADER_PATH src/audio/headers)
set(SRC_GRAPHICS_HEADER_PATH src/graphics/headers)
set(SRC_OPENGL_HEADER_PATH src/opengl/headers)
set(SRC_INPUT_HEADER_PATH src/input/headers)
set(SRC_FS_HEADER_PATH src/filesystem/headers)

set(SRC_AUDIO_SOURCE_PATH src/audio/source)
set(SRC_GRAPHICS_SOURCE_PATH src/graphics/source)
set(SRC_OPENGL_SOURCE_PATH src/opengl/source)
set(SRC_INPUT_SOURCE_PATH src/input/source)
set(SRC_FS_SOURCE_PATH src/filesystem/source)

set(SRC_STEAM_PATH src/steam)

set(MAIN_HEADERS
	# Audio
	${SRC_AUDIO_HEADER_PATH}/audio.h
	${SRC_AUDIO_HEADER_PATH}/al-util.h
	${SRC_AUDIO_HEADER_PATH}/aldatasource.h
	${SRC_AUDIO_HEADER_PATH}/alstream.h
	${SRC_AUDIO_HEADER_PATH}/audiostream.h
	${SRC_AUDIO_HEADER_PATH}/audiochannels.h
	${SRC_AUDIO_HEADER_PATH}/soundemitter.h

	# Graphics
	${SRC_GRAPHICS_HEADER_PATH}/bitmap.h
	${SRC_GRAPHICS_HEADER_PATH}/graphics.h
	${SRC_GRAPHICS_HEADER_PATH}/disposable.h
	${SRC_GRAPHICS_HEADER_PATH}/font.h
	${SRC_GRAPHICS_HEADER_PATH}/sprite.h
	${SRC_GRAPHICS_HEADER_PATH}/scene.h
	${SRC_GRAPHICS_HEADER_PATH}/tilemap.h
	${SRC_GRAPHICS_HEADER_PATH}/tilemap-common.h
	${SRC_GRAPHICS_HEADER_PATH}/tileatlas.h

	# OpenGL
	${SRC_OPENGL_HEADER_PATH}/glstate.h
	${SRC_OPENGL_HEADER_PATH}/gl-debug.h
	${SRC_OPENGL_HEADER_PATH}/gl-util.h
	${SRC_OPENGL_HEADER_PATH}/gl-fun.h
	${SRC_OPENGL_HEADER_PATH}/gl-meta.h
	${SRC_OPENGL_HEADER_PATH}/quadarray.h
	${SRC_OPENGL_HEADER_PATH}/plane.h
	${SRC_OPENGL_HEADER_PATH}/shader.h
	${SRC_OPENGL_HEADER_PATH}/quad.h
	${SRC_OPENGL_HEADER_PATH}/texpool.h
	${SRC_OPENGL_HEADER_PATH}/tilequad.h
	${SRC_OPENGL_HEADER_PATH}/vertex.h
	${SRC_OPENGL_HEADER_PATH}/window.h
	${SRC_OPENGL_HEADER_PATH}/sdl-util.h
	${SRC_OPENGL_HEADER_PATH}/viewport.h
	${SRC_OPENGL_HEADER_PATH}/table.h
	${SRC_OPENGL_HEADER_PATH}/transform.h

	# Input
	${SRC_INPUT_HEADER_PATH}/input.h
	${SRC_INPUT_HEADER_PATH}/keybindings.h

	# Filesystem
	${SRC_FS_HEADER_PATH}/filesystem.h
	${SRC_FS_HEADER_PATH}/rgssad.h

	src/binding.h
	src/etc.h
	src/etc-internal.h
	src/eventthread.h
	src/flashable.h
	src/serializable.h
	src/global-ibo.h
	src/exception.h
	src/serial-util.h
	src/intrulist.h
	src/binding.h
	src/util.h
	src/config.h
	src/settingsmenu.h
	src/sharedstate.h
	src/boost-hash.h
	src/debugwriter.h
	src/oneshot.h
	src/pipe.h
	chromasdk/RzChromaSDKDefines.h
	chromasdk/RzChromaSDKTypes.h
	chromasdk/RzErrors.h
	chromasdk/ChromaApi.h
	src/i18n.h
	src/otherview-message.h
	src/crash-handler.h
)

set(MAIN_SOURCE
	# Audio
	${SRC_AUDIO_SOURCE_PATH}/alstream.cpp
	${SRC_AUDIO_SOURCE_PATH}/audiostream.cpp
	${SRC_AUDIO_SOURCE_PATH}/audiochannels.cpp
	${SRC_AUDIO_SOURCE_PATH}/audio.cpp
	${SRC_AUDIO_SOURCE_PATH}/soundemitter.cpp
	${SRC_AUDIO_SOURCE_PATH}/sdlsoundsource.cpp
	${SRC_AUDIO_SOURCE_PATH}/vorbissource.cpp

	# Graphics
	${SRC_GRAPHICS_SOURCE_PATH}/autotiles.cpp
	${SRC_GRAPHICS_SOURCE_PATH}/bitmap.cpp
	${SRC_GRAPHICS_SOURCE_PATH}/graphics.cpp
	${SRC_GRAPHICS_SOURCE_PATH}/font.cpp
	${SRC_GRAPHICS_SOURCE_PATH}/sprite.cpp
	${SRC_GRAPHICS_SOURCE_PATH}/scene.cpp
	${SRC_GRAPHICS_SOURCE_PATH}/tilemap.cpp
	${SRC_GRAPHICS_SOURCE_PATH}/tileatlas.cpp

	# OpenGL
	${SRC_OPENGL_SOURCE_PATH}/glstate.cpp
	${SRC_OPENGL_SOURCE_PATH}/gl-debug.cpp
	${SRC_OPENGL_SOURCE_PATH}/gl-fun.cpp
	${SRC_OPENGL_SOURCE_PATH}/gl-meta.cpp
	${SRC_OPENGL_SOURCE_PATH}/plane.cpp
	${SRC_OPENGL_SOURCE_PATH}/shader.cpp
	${SRC_OPENGL_SOURCE_PATH}/texpool.cpp
	${SRC_OPENGL_SOURCE_PATH}/vertex.cpp
	${SRC_OPENGL_SOURCE_PATH}/tilequad.cpp
	${SRC_OPENGL_SOURCE_PATH}/window.cpp
	${SRC_OPENGL_SOURCE_PATH}/screen.cpp
	${SRC_OPENGL_SOURCE_PATH}/viewport.cpp
	${SRC_OPENGL_SOURCE_PATH}/table.cpp

	# Input
	${SRC_INPUT_SOURCE_PATH}/input.cpp
	${SRC_INPUT_SOURCE_PATH}/keybindings.cpp

	# Filesystem
	${SRC_FS_SOURCE_PATH}/filesystem.cpp
	${SRC_FS_SOURCE_PATH}/rgssad.cpp

	src/main.cpp
	src/eventthread.cpp
	src/etc.cpp
	src/config.cpp
	src/settingsmenu.cpp
	src/sharedstate.cpp
	src/oneshot.cpp
	src/i18n.cpp
	src/otherview-message.cpp
	src/crash-handler.cpp
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
	add_compile_options(${LINUXPKGS_CFLAGS_OTHER} -g)
	list(APPEND PLATFORM_LIBRARIES ${LINUXPKGS_LDFLAGS})
	list(APPEND MAIN_SOURCE src/xdg-user-dir-lookup.c)
	list(APPEND MAIN_HEADERS src/xdg-user-dir-lookup.h)
endif()

if (STEAM)
	list(APPEND MAIN_HEADERS
		${SRC_STEAM_PATH}/steam.h
		steamshim/steamshim_child.h
	)
	list(APPEND MAIN_SOURCE
		${SRC_STEAM_PATH}/steam.cpp
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
	shader/mask.frag
	shader/mask.vert
	shader/crt.frag
	shader/crt_sprite.frag
	shader/simpleMatrix.vert
	shader/chronos.frag
	shader/cubic_lens.frag
	shader/water.frag
	shader/zoom.vert
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
	binding-mri/aleffect-binding.cpp
	binding-mri/otherview-binding.cpp
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
target_include_directories(${PROJECT_NAME} PRIVATE ${SRC_AUDIO_HEADER_PATH})
target_include_directories(${PROJECT_NAME} PRIVATE ${SRC_GRAPHICS_HEADER_PATH})
target_include_directories(${PROJECT_NAME} PRIVATE ${SRC_OPENGL_HEADER_PATH})
target_include_directories(${PROJECT_NAME} PRIVATE ${SRC_INPUT_HEADER_PATH})
target_include_directories(${PROJECT_NAME} PRIVATE ${SRC_FS_HEADER_PATH})
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
	CONAN_PKG::cppzmq
	${PLATFORM_LIBRARIES})

IF(APPLE)
	add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD COMMAND cmake -P "${CMAKE_SOURCE_DIR}/patches/mac/CompleteBundle.cmake" VERBATIM)
ENDIF()

if(WIN32 AND NOT STEAM)
	add_executable("oneshot-shim" WIN32
	"${CMAKE_SOURCE_DIR}/windows/shim.c"
	"${CMAKE_SOURCE_DIR}/assets/resources.rc")
endif()

#add_subdirectory(scripts)
