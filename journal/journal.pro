TEMPLATE = app
CONFIG += c++11
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += main.cpp \
    niko.cpp \
    journal.cpp

LIBS += -lgdi32

RC_FILE = resources.rc
