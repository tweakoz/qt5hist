SOURCES += tst_subtest.cpp
QT = core testlib

mac:CONFIG -= app_bundle
CONFIG -= debug_and_release_target

TARGET = subtest
DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0
