QT       += core gui widgets

DEFINES += QT_DEPRECATED_WARNINGS

CONFIG += link_pkgconfig
PKGCONFIG += dtk6widget

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

isEmpty(PREFIX): PREFIX = /usr
target.path = $$PREFIX/bin
desktop.files = passwordgenerator.desktop
desktop.path = $$PREFIX/share/applications
icon.files = passwordgenerator.svg
icon.path = $$PREFIX/share/icons/hicolor/scalable/apps
INSTALLS += target desktop icon
