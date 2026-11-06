QT += core gui widgets
CONFIG += c++17 link_pkgconfig lrelease embed_translations
PKGCONFIG += dtk6widget
DEFINES += QT_DEPRECATED_WARNINGS

TEMPLATE = app
TARGET = passwordgenerator

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    widget.cpp \
    generador.cpp

HEADERS += \
    mainwindow.h \
    widget.h \
    generador.h

RESOURCES += ../data/passwordgenerator.qrc

# Traducciones: se compilan con lrelease y se embeben en :/i18n
TRANSLATIONS += \
    ../translations/passwordgenerator_en.ts \
    ../translations/passwordgenerator_pt.ts \
    ../translations/passwordgenerator_fr.ts \
    ../translations/passwordgenerator_it.ts \
    ../translations/passwordgenerator_zh_CN.ts

isEmpty(PREFIX): PREFIX = /usr
target.path = $$PREFIX/bin
desktop.files = $$PWD/../data/passwordgenerator.desktop
desktop.path = $$PREFIX/share/applications
icon.files = $$PWD/../data/passwordgenerator.svg
icon.path = $$PREFIX/share/icons/hicolor/scalable/apps
INSTALLS += target desktop icon
