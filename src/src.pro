#-------------------------------------------------
#
# Project created by QtCreator 2010-10-15T15:03:16
#
#-------------------------------------------------

TEMPLATE = app
TARGET = conductor

CONFIG      += link_pkgconfig qxt warn_on
PKGCONFIG   += libpulse-mainloop-glib

QXT = core network

SOURCES += main.cpp \
    mainwindow.cpp \
    moduleloader.cpp \
    papplication.cpp \
    algo.cpp \
    pacontroller.cpp \
    sinkinputmodel.cpp \
    sinkinput.cpp \
    sink.cpp \
    sinkmodel.cpp

HEADERS += \
    mainwindow.h \
    moduleloader.h \
    pacontroller.h \
    sinkinputmodel.h \
    sinkinput.h \
    sink.h \
    sinkmodel.h

FORMS   += \
    mainwindow.ui
