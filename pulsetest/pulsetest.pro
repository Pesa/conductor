TEMPLATE = app
TARGET = pulsetest
QT = core

CONFIG += debug link_pkgconfig warn_on
PKGCONFIG += libpulse-mainloop-glib

SOURCES = main.cpp
