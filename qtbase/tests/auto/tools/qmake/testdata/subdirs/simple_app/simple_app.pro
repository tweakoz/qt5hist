TEMPLATE		= app
CONFIG		+= qt warn_on
HEADERS		= test_file.h
SOURCES		= test_file.cpp \
		  	main.cpp
TARGET		= simple_app
DESTDIR		= ./
DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0
