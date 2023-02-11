TEMPLATE = app
TARGET = DelayTask
DESTDIR = ./build

QT += core widgets gui

HEADERS += ./DelayTask.h \
    Dialog.h

SOURCES += ./main.cpp \
    DelayTask.cpp \
    Dialog.cpp

    

FORMS += \
    Dialog.ui
