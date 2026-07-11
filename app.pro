QT       += core gui svg
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

DEFINES += QT_DEPRECATED_WARNINGS

CONFIG += link_pkgconfig
PKGCONFIG += dtkwidget

TEMPLATE = app
TARGET = passwordgenerator

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    widget.cpp

HEADERS += \
    mainwindow.h \
    widget.h

FORMS += \
    widget.ui

RESOURCES += \
    imgs.qrc