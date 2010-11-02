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
    algo.cpp \
    pacontroller.cpp \
    sinkinputmodel.cpp \
    sinkinput.cpp \
    sink.cpp \
    sinkmodel.cpp \
    probeinterface.cpp \
    probemanager.cpp

HEADERS += \
    mainwindow.h \
    pacontroller.h \
    sinkinputmodel.h \
    sinkinput.h \
    sink.h \
    sinkmodel.h \
    probeinterface.h \
    probemanager.h

FORMS   += \
    mainwindow.ui
