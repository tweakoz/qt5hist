CONFIG += testcase
TARGET = tst_dialogs
SOURCES += tst_dialogs.cpp

include (../../shared/util.pri)

macx:CONFIG -= app_bundle
macx:CONFIG+=insignificant_test    # QTBUG-30513 - test is unstable
linux-*:CONFIG+=insignificant_test    # QTBUG-30513 - test is unstable

CONFIG += parallel_test
QT += core-private gui-private qml-private quick-private v8-private testlib

TESTDATA = data/*

OTHER_FILES += \
    data/FileDialog.qml \

DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0
