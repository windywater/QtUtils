TEMPLATE = app
TARGET = SpectrumChartView
DESTDIR = ./build

QT += core widgets gui charts

HEADERS += ./CommonChartView.h \
    ./ImageChartView.h \
	./SpectrumChartView.h \
    ./LinearColor.h

SOURCES += ./main.cpp \
    ./CommonChartView.cpp \
    ./ImageChartView.cpp \
	./SpectrumChartView.cpp \
    ./LinearColor.cpp
