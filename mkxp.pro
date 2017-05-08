

TEMPLATE = app
QT =
TARGET = OneShot
DEPENDPATH += src shader assets
INCLUDEPATH += . src

CONFIG -= qt
CONFIG += link_pkgconfig

CONFIG(release, debug|release): DEFINES += NDEBUG

isEmpty(BINDING) {
	BINDING = MRI
}

contains(BINDING, MRI) {
	contains(_HAVE_BINDING, YES) {
		error("Only one binding may be selected")
	}
	_HAVE_BINDING = YES

	CONFIG += BINDING_MRI
}

contains(BINDING, NULL) {
	contains(_HAVE_BINDING, YES) {
		error("Only one binding may be selected")
	}
	_HAVE_BINDING = YES

	CONFIG += BINDING_NULL
}

unix {
	CONFIG += c++11
	PKGCONFIG += sigc++-2.0 pixman-1 vorbisfile \
				 sdl2 SDL2_image SDL2_ttf SDL_sound physfs
	LIBS += -ldl
	macx: {
		INCLUDEPATH += $$QMAKE_MAC_SDK_PATH/System/Library/Frameworks/OpenAL.framework/Versions/A/Headers
		LIBS += -framework OpenAL
		QMAKE_LFLAGS += -L/usr/local/opt/ruby@2.3/lib -L/usr/local/opt/openal-soft/lib
	}
	!macx: {
		PKGCONFIG += openal zlib
		INCLUDEPATH += /usr/include/AL /usr/local/include/AL
		SOURCES += src/xdg-user-dir-lookup.c
		LIBS += -lX11
	}
}

win32 {
	QMAKE_CXXFLAGS += -std=gnu++11
	QMAKE_LFLAGS += -std=gnu++11

	PKGCONFIG += sigc++-2.0 pixman-1 zlib \
	             sdl2 SDL2_image SDL2_ttf openal SDL_sound vorbisfile freetype2
	LIBS += -lphysfs -lsecur32 -lwinmm

	release {
	    RC_FILE = assets/resources.rc
	}
}

# Deal with boost paths...
isEmpty(BOOST_I) {
	BOOST_I = $$(BOOST_I)
}
isEmpty(BOOST_I) {}
else {
	INCLUDEPATH += $$BOOST_I
}

isEmpty(BOOST_L) {
	BOOST_L = $$(BOOST_L)
}
isEmpty(BOOST_L) {}
else {
	LIBS += -L$$BOOST_L
}

isEmpty(BOOST_LIB_SUFFIX) {
	BOOST_LIB_SUFFIX = $$(BOOST_LIB_SUFFIX)
}

LIBS += -lboost_program_options$$BOOST_LIB_SUFFIX

STEAM {
	DEFINES += STEAM
}

# Input
HEADERS += \
	src/quadarray.h \
	src/audio.h \
	src/binding.h \
	src/bitmap.h \
	src/disposable.h \
	src/etc.h \
	src/etc-internal.h \
	src/eventthread.h \
	src/flashable.h \
	src/font.h \
	src/input.h \
	src/plane.h \
	src/scene.h \
	src/sprite.h \
	src/table.h \
	src/texpool.h \
	src/tilequad.h \
	src/transform.h \
	src/viewport.h \
	src/window.h \
	src/serializable.h \
	src/shader.h \
	src/glstate.h \
	src/quad.h \
	src/tilemap.h \
	src/tilemap-common.h \
	src/graphics.h \
	src/gl-debug.h \
	src/global-ibo.h \
	src/exception.h \
	src/filesystem.h \
	src/serial-util.h \
	src/intrulist.h \
	src/binding.h \
	src/gl-util.h \
	src/util.h \
	src/config.h \
	src/settingsmenu.h \
	src/keybindings.h \
	src/tileatlas.h \
	src/sharedstate.h \
	src/al-util.h \
	src/boost-hash.h \
	src/debugwriter.h \
	src/gl-fun.h \
	src/gl-meta.h \
	src/vertex.h \
	src/soundemitter.h \
	src/aldatasource.h \
	src/alstream.h \
	src/audiostream.h \
	src/rgssad.h \
	src/sdl-util.h \
	src/oneshot.h \
	src/pipe.h

SOURCES += \
	src/main.cpp \
	src/audio.cpp \
	src/bitmap.cpp \
	src/eventthread.cpp \
	src/filesystem.cpp \
	src/font.cpp \
	src/input.cpp \
	src/plane.cpp \
	src/scene.cpp \
	src/sprite.cpp \
	src/table.cpp \
	src/tilequad.cpp \
	src/viewport.cpp \
	src/window.cpp \
	src/texpool.cpp \
	src/shader.cpp \
	src/glstate.cpp \
	src/tilemap.cpp \
	src/autotiles.cpp \
	src/graphics.cpp \
	src/gl-debug.cpp \
	src/etc.cpp \
	src/config.cpp \
	src/settingsmenu.cpp \
	src/keybindings.cpp \
	src/tileatlas.cpp \
	src/sharedstate.cpp \
	src/gl-fun.cpp \
	src/gl-meta.cpp \
	src/vertex.cpp \
	src/soundemitter.cpp \
	src/sdlsoundsource.cpp \
	src/alstream.cpp \
	src/audiostream.cpp \
	src/rgssad.cpp \
	src/vorbissource.cpp \
	src/oneshot.cpp \
	src/screen.cpp \
    binding-mri/journal-binding.cpp

STEAM {
	HEADERS += src/steam.h steamshim/steamshim_child.h
	SOURCES += src/steam.cpp steamshim/steamshim_child.c
}

EMBED = \
	shader/common.h \
	shader/transSimple.frag \
	shader/trans.frag \
	shader/hue.frag \
	shader/sprite.frag \
	shader/plane.frag \
	shader/gray.frag \
	shader/bitmapBlit.frag \
	shader/flatColor.frag \
	shader/simple.frag \
	shader/simpleColor.frag \
	shader/simpleAlpha.frag \
	shader/simpleAlphaUni.frag \
	shader/flashMap.frag \
	shader/obscured.frag \
	shader/minimal.vert \
	shader/simple.vert \
	shader/simpleColor.vert \
	shader/sprite.vert \
	shader/tilemap.vert \
	shader/blur.frag \
	shader/blurH.vert \
	shader/blurV.vert \
	shader/simpleMatrix.vert \
	assets/icon.png \
	assets/gamecontrollerdb.txt

defineReplace(xxdOutput) {
	return($$basename(1).xxd)
}

# xxd
xxd.output_function = xxdOutput
xxd.commands = xxd -i ${QMAKE_FILE_NAME} > ${QMAKE_FILE_OUT}
xxd.depends = $$EMBED
xxd.input = EMBED
xxd.variable_out = HEADERS

QMAKE_EXTRA_COMPILERS += xxd


BINDING_NULL {
	SOURCES += binding-null/binding-null.cpp
}

BINDING_MRI {
	isEmpty(MRIVERSION) {
		MRIVERSION = 2.3
	}

	PKGCONFIG += ruby-$$MRIVERSION
	DEFINES += BINDING_MRI

#	EMBED2 = binding-mri/module_rpg.rb
#	xxdp.output = ${QMAKE_FILE_NAME}.xxd
#	xxdp.commands = xxd+/xxd+ ${QMAKE_FILE_NAME} -o ${QMAKE_FILE_OUT} --string
#	xxdp.depends = $$EMBED2
#	xxdp.input = EMBED2
#	xxdp.variable_out = HEADERS
#	QMAKE_EXTRA_COMPILERS += xxdp

	HEADERS += \
	binding-mri/binding-util.h \
	binding-mri/binding-types.h \
	binding-mri/serializable-binding.h \
	binding-mri/disposable-binding.h \
	binding-mri/sceneelement-binding.h \
	binding-mri/viewportelement-binding.h \
	binding-mri/flashable-binding.h

	SOURCES += \
	binding-mri/binding-mri.cpp \
	binding-mri/binding-util.cpp \
	binding-mri/table-binding.cpp \
	binding-mri/etc-binding.cpp \
	binding-mri/bitmap-binding.cpp \
	binding-mri/font-binding.cpp \
	binding-mri/graphics-binding.cpp \
	binding-mri/input-binding.cpp \
	binding-mri/sprite-binding.cpp \
	binding-mri/viewport-binding.cpp \
	binding-mri/plane-binding.cpp \
	binding-mri/window-binding.cpp \
	binding-mri/tilemap-binding.cpp \
	binding-mri/audio-binding.cpp \
	binding-mri/module_rpg.cpp \
	binding-mri/filesystem-binding.cpp \
	binding-mri/oneshot-binding.cpp \
	binding-mri/steam-binding.cpp \
	binding-mri/wallpaper-binding.cpp \
	binding-mri/niko-binding.cpp
}

OTHER_FILES += $$EMBED
