CONFIG += testcase
TARGET = tst_qdeclarativeanimatedimage

QT += testlib
contains(QT_CONFIG,declarative): QT += declarative declarative-private network widgets
HEADERS += ../shared/testhttpserver.h
SOURCES += tst_qdeclarativeanimatedimage.cpp ../shared/testhttpserver.cpp
macx:CONFIG -= app_bundle

include(../shared/qdeclarativedatatest.pri)

CONFIG += parallel_test
DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0
