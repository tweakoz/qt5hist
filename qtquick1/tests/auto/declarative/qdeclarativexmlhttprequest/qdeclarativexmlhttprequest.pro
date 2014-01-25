CONFIG += testcase
testcase.timeout = 1200 # this is a slow test
TARGET = tst_qdeclarativexmlhttprequest

QT += testlib
contains(QT_CONFIG,declarative): QT += declarative network
macx:CONFIG -= app_bundle

HEADERS += ../shared/testhttpserver.h

SOURCES += tst_qdeclarativexmlhttprequest.cpp \
           ../shared/testhttpserver.cpp

include(../shared/qdeclarativedatatest.pri)

DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0
