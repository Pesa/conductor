#-------------------------------------------------
#
# Project created by QtCreator 2010-10-04T23:01:53
#
#-------------------------------------------------

TEMPLATE = app
TARGET = probe

CONFIG  -= app_bundle
CONFIG  += console qxt warn_on

QT  = core dbus
QXT = core network

SOURCES += main.cpp \
    probe.cpp

HEADERS += \
    probe.h
