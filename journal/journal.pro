TEMPLATE = app
CONFIG -= app_bundle qt
QMAKE_CFLAGS += -std=gnu11

# Linux
unix:!macx {
	SOURCES += \
		linux/main.c \
		linux/niko.c \
		linux/journal.c \
		linux/image/niko1.c \
		linux/image/niko2.c \
		linux/image/niko3.c \
		linux/image/default.c \
		linux/image/c1.c \
		linux/image/c2.c \
		linux/image/c3.c \
		linux/image/c4.c \
		linux/image/c5.c \
		linux/image/c6.c \
		linux/image/c7.c \
		linux/image/s1.c \
		linux/image/s2.c \
		linux/image/s3.c \
		linux/image/s4.c \
		linux/image/t1.c \
		linux/image/t2.c \
		linux/image/t3.c \
		linux/image/t4.c \
		linux/image/t5.c \
		linux/image/t6.c \
		linux/image/t7.c \
		linux/image/t8.c \
		linux/image/t9.c \
		linux/image/t10.c \
		linux/image/t11.c \
		linux/image/t12.c \
		linux/image/t13.c \
		linux/image/t14.c \
		linux/image/t15.c \
		linux/image/t16.c \
		linux/image/final.c \
		linux/image/save.c

	HEADERS += \
		linux/display.h \
		linux/image.h

	QMAKE_CFLAGS += -pthread

	LIBS += -pthread -lxcb -lxcb-util -lxcb-image -lxcb-shape -lxcb-icccm -lxcb-ewmh
}

# OS X
macx {
	# Directly copied from Linux, assuming this works but unsure

	SOURCES += \
		mac/main.c \
		mac/niko.c \
		mac/journal.c \
		mac/image/niko1.c \
		mac/image/niko2.c \
		mac/image/niko3.c \
		mac/image/default.c \
		mac/image/c1.c \
		mac/image/c2.c \
		mac/image/c3.c \
		mac/image/c4.c \
		mac/image/c5.c \
		mac/image/c6.c \
		mac/image/c7.c \
		mac/image/s1.c \
		mac/image/s2.c \
		mac/image/s3.c \
		mac/image/s4.c \
		mac/image/t1.c \
		mac/image/t2.c \
		mac/image/t3.c \
		mac/image/t4.c \
		mac/image/t5.c \
		mac/image/t6.c \
		mac/image/t7.c \
		mac/image/t8.c \
		mac/image/t9.c \
		mac/image/t10.c \
		mac/image/t11.c \
		mac/image/t12.c \
		mac/image/t13.c \
		mac/image/t14.c \
		mac/image/t15.c \
		mac/image/t16.c \
		mac/image/final.c \
		mac/image/save.c

	HEADERS += \
		mac/display.h \
		mac/image.h

	QMAKE_CFLAGS += -pthread

	LIBS += -pthread -lxcb -lxcb-util -lxcb-image -lxcb-shape -lxcb-icccm -lxcb-ewmh
}

# Windows
win32 {
	SOURCES += \
		w32/main.c \
		w32/niko.c \
		w32/journal.c

	LIBS += -lgdi32 -lshlwapi

	RC_FILE = w32/resources.rc
}
