QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = DeepVikApp

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

QMAKE_CXXFLAGS += -march=native -mpopcnt -Ofast -fomit-frame-pointer
QMAKE_CFLAGS += -march=native -mpopcnt -Ofast -fomit-frame-pointer

SOURCES += \
    main.cpp \
    DeepVik.cpp \
    src/myBot.cpp \
    src/moveGenerator.cpp

HEADERS += \
    DeepVik.h \
    src/myBot.h \
    src/moveGenerator.h

FORMS += \
    deepvik.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    images.qrc
