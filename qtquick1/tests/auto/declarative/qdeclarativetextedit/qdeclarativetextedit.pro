CONFIG += testcase
TARGET = tst_qdeclarativetextedit

QT += testlib
contains(QT_CONFIG,declarative): QT += declarative declarative-private gui gui-private widgets widgets-private network core-private
macx:CONFIG -= app_bundle

SOURCES += tst_qdeclarativetextedit.cpp ../shared/testhttpserver.cpp
HEADERS += ../shared/testhttpserver.h

DEFINES += SRCDIR=\\\"$$PWD\\\"
DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0
