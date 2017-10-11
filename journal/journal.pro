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
}

# Windows
win32 {
	SOURCES += \
		w32/main.c \
		w32/niko.c \
		w32/journal.c

	LIBS += -lgdi32 -lshell32 -luuid -lole32

	RC_FILE = w32/resources.rc
}
