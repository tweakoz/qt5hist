############################################################
# Project file for autotest for file qstackedwidget.h
############################################################

CONFIG += testcase
CONFIG += parallel_test
TARGET = tst_qstackedwidget
QT += widgets testlib
SOURCES += tst_qstackedwidget.cpp


DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0
