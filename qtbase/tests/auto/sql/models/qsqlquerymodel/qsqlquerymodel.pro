CONFIG += testcase
TARGET = tst_qsqlquerymodel
SOURCES  += tst_qsqlquerymodel.cpp

QT += widgets sql testlib

wince*: {
   DEPLOYMENT_PLUGIN += qsqlite
	LIBS += -lws2
} else {
   win32:LIBS += -lws2_32
}

DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0
