QT       += core gui widgets testlib

DEFINES += QT_DEPRECATED_WARNINGS
CONFIG += c++17

CONFIG += link_pkgconfig console testcase no_testcase_installs
PKGCONFIG += dtk6widget

TEMPLATE = app
TARGET = test_widget

SOURCES += \
    test_widget.cpp \
    widget.cpp \
    generador.cpp

HEADERS += \
    widget.h \
    generador.h


RESOURCES += \
    imgs.qrc
