QT += core gui widgets

TARGET = ScreenshotTool
TEMPLATE = app

CONFIG += c++11 release
DESTDIR = release

# УДАЛЕНА проблемная строка: win32:RC_FILE = app.rc
# (файл app.rc отсутствует в репозитории)

SOURCES += \
    main.cpp \
    screenshottool.cpp \
    regionselector.cpp

HEADERS += \
    screenshottool.h \
    regionselector.h \
    themes.h

# Для 64-битной сборки
win32 {
    CONFIG += windows  # Гарантирует правильную точку входа WinMain
    QMAKE_CXXFLAGS += -Wno-deprecated-copy  # Подавляем шумные предупреждения
}
