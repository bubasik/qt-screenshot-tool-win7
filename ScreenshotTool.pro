QT += core gui widgets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = ScreenshotTool
TEMPLATE = app

CONFIG += c++11

SOURCES += \
    main.cpp \
    screenshottool.cpp

HEADERS += \
    screenshottool.h \
    themes.h
