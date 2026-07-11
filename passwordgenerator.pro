QT       += core gui svg testlib

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

DEFINES += QT_DEPRECATED_WARNINGS

CONFIG += link_pkgconfig
PKGCONFIG += dtkwidget

TEMPLATE = subdirs
SUBDIRS = app testlib

app.file = app.pro
testlib.file = testlib.pro
testlib.depends = app
