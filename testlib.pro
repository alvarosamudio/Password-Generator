QT       += core gui svg widgets testlib

DEFINES += QT_DEPRECATED_WARNINGS

CONFIG += link_pkgconfig console
PKGCONFIG += dtkwidget

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