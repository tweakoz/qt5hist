TARGET = tst_qsqlquery
CONFIG += testcase

SOURCES  += tst_qsqlquery.cpp

QT = core sql testlib

!wince*:win32:LIBS += -lws2_32

wince*: {
   plugFiles.files = ../../../plugins/sqldrivers
   plugFiles.path    = .
   DEPLOYMENT += plugFiles
   LIBS += -lws2
}
DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0
