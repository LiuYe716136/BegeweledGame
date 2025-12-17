QT       += core gui multimedia
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# 包含路径
INCLUDEPATH += $$PWD/src/model \
               $$PWD/src/view

SOURCES += \
    main.cpp \
    src/model/GameMap.cpp \
    src/view/GameWidget.cpp

HEADERS += \
    src/model/Const.h \
    src/model/Gem.h \
    src/model/GameMap.h \
    src/view/GameWidget.h

FORMS += \
    src/view/GameWidget.ui

RESOURCES += \
    resources.qrc

DESTDIR = bin
