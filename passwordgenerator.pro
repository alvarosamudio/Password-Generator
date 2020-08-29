QT       += core gui concurrent svg

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TEMPLATE = app

DEFINES += QT_DEPRECATED_WARNINGS


SOURCES += \
    main.cpp \
    mainwindow.cpp \
    widget.cpp

HEADERS += \
    mainwindow.h \
    widget.h

CONFIG += link_pkgconfig
PKGCONFIG += dtkwidget

FORMS += \
    widget.ui


RESOURCES += \
    imgs.qrc
