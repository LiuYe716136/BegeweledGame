QT       += core gui multimedia
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# 包含路径
INCLUDEPATH += $$PWD/src/model \
               $$PWD/src/view

SOURCES += \
    main.cpp \
    src/model/GameMap.cpp \
    src/view/GameWidget.cpp \
    src/view/MenuWidget.cpp \
    src/view/RankingWidget.cpp

HEADERS += \
    src/model/Const.h \
    src/model/Gem.h \
    src/model/GameMap.h \
    src/view/GameWidget.h \
    src/view/MenuWidget.h \
    src/view/RankingWidget.h

FORMS += \
    src/view/GameWidget.ui \
    src/view/MenuWidget.ui \
    src/view/RankingWidget.ui

RESOURCES += \
    resources.qrc

DESTDIR = bin
