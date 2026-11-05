QT       += core gui widgets testlib


DEFINES += QT_DEPRECATED_WARNINGS

CONFIG += link_pkgconfig
PKGCONFIG += dtk6widget

TEMPLATE = subdirs
SUBDIRS = app testlib

app.file = app.pro
testlib.file = testlib.pro
testlib.depends = app
