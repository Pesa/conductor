TEMPLATE = app
TARGET = conductor

CONFIG      += link_pkgconfig qxt warn_on
PKGCONFIG   += libpulse-mainloop-glib

QXT = core network


SOURCES += main.cpp \
    mainwindow.cpp \
    algorithm.cpp \
    pacontroller.cpp \
    paoperation.cpp \
    paoperationqueue.cpp \
    sinkinputmodel.cpp \
    sinkinput.cpp \
    sink.cpp \
    sinkmodel.cpp \
    streamproxymodel.cpp \
    probeinterface.cpp \
    probemanager.cpp \
    rssimodel.cpp \
    config.cpp

HEADERS += \
    mainwindow.h \
    algorithm.h \
    pacontroller.h \
    paoperation.h \
    paoperationqueue.h \
    sinkinputmodel.h \
    sinkinput.h \
    sink.h \
    sinkmodel.h \
    streamproxymodel.h \
    probeinterface.h \
    probemanager.h \
    rssimodel.h \
    config.h

FORMS   += \
    mainwindow.ui
