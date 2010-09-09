TEMPLATE = app
TARGET = bluetest
QT = core

CONFIG += debug link_pkgconfig warn_on
PKGCONFIG += bluedevil

HEADERS = bluetest.h
SOURCES = bluetest.cpp main.cpp
