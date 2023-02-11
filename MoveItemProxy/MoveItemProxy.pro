TEMPLATE = app
TARGET = MoveItemProxy
DESTDIR = ./build

QT += core widgets gui

HEADERS += ./MoveItemProxy.h \
    Dialog.h

SOURCES += ./main.cpp \
    MoveItemProxy.cpp \
    Dialog.cpp

FORMS += \
    Dialog.ui
