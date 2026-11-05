QT       += core gui widgets testlib

DEFINES += QT_DEPRECATED_WARNINGS

CONFIG += link_pkgconfig console testcase no_testcase_installs
PKGCONFIG += dtk6widget

TEMPLATE = app
TARGET = test_widget

SOURCES += \
    test_widget.cpp \
    widget.cpp

HEADERS += \
    widget.h

FORMS += \
    widget.ui

RESOURCES += \
    imgs.qrc
