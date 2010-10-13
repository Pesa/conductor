TEMPLATE = app
TARGET = pulseapp
QT = core

CONFIG += debug link_pkgconfig warn_on
PKGCONFIG += libpulse-mainloop-glib

HEADERS = SinkViewer.h \
	SinkInputViewer.h \
	ClientViewer.h \
	ModuleLoader.h

SOURCES = PApplication.cpp \
	SinkViewer.cpp \
	SinkInputViewer.cpp \
	ClientViewer.cpp \
	ModuleLoader.cpp
