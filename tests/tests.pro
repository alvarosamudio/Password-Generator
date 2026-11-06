QT += core gui widgets testlib
CONFIG += c++17 link_pkgconfig console testcase no_testcase_installs
PKGCONFIG += dtk6widget
DEFINES += QT_DEPRECATED_WARNINGS

TEMPLATE = app
TARGET = test_widget

INCLUDEPATH += ../src
SOURCES += \
    test_widget.cpp \
    ../src/widget.cpp \
    ../src/generador.cpp
HEADERS += \
    ../src/widget.h \
    ../src/generador.h
RESOURCES += ../data/passwordgenerator.qrc
