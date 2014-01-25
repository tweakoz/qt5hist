CONFIG += testcase
TARGET = tst_qsqlthread
SOURCES  += tst_qsqlthread.cpp

QT = core sql testlib


wince*: {
   plugFiles.files = ../../../plugins/sqldrivers
   plugFiles.path    = .
   DEPLOYMENT += plugFiles
   LIBS += -lws2
} else {
   win32:LIBS += -lws2_32
}

DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0
