QT += core gui widgets

TARGET = ScreenshotTool
TEMPLATE = app

CONFIG += c++11 release
DESTDIR = release

SOURCES += \
    main.cpp \
    screenshottool.cpp \
    regionselector.cpp

HEADERS += \
    screenshottool.h \
    regionselector.h \
    themes.h

# Для Windows: иконка приложения (опционально)
win32:RC_FILE = app.rc
