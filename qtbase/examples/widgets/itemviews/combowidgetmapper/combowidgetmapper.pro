HEADERS   = window.h
SOURCES   = main.cpp \
            window.cpp

# install
target.path = $$[QT_INSTALL_EXAMPLES]/widgets/itemviews/combowidgetmapper
INSTALLS += target
QT += widgets
