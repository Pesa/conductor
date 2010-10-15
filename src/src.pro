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
    clientviewer.cpp \
    moduleloader.cpp \
    sinkinputviewer.cpp \
    sinkviewer.cpp \
    papplication.cpp \
    algo.cpp

HEADERS += \
    mainwindow.h \
    clientviewer.h \
    moduleloader.h \
    sinkinputviewer.h \
    sinkviewer.h

FORMS   += \
    mainwindow.ui
