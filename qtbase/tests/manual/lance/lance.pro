LANCELOT_DIR = $$PWD/../../auto/other/lancelot
CONFIG+=console moc
TEMPLATE = app
INCLUDEPATH += . $$LANCELOT_DIR
QT += core-private gui-private widgets printsupport

HEADERS += widgets.h \
           interactivewidget.h \
           $$LANCELOT_DIR/paintcommands.h
SOURCES += interactivewidget.cpp \
           main.cpp \
           $$LANCELOT_DIR/paintcommands.cpp
RESOURCES += icons.qrc \
           $$LANCELOT_DIR/images.qrc

contains(QT_CONFIG, opengl)|contains(QT_CONFIG, opengles1)|contains(QT_CONFIG, opengles2):QT += opengl

DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0
