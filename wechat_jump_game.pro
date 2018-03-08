#-------------------------------------------------
#
# Project created by QtCreator 2018-01-03T10:12:18
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = wechat_jump_game
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += \
        main.cpp \
        mainwindow.cpp \
    jumpjump.cpp \
    QImageMatConvert/mat_qimage_convert.cpp \
    ImageWidget-Qt/ImageWidget.cpp \
    ImageWidget-Qt/selectrect.cpp \
    jumpcontroller.cpp \
    fixparameterdialog.cpp

HEADERS += \
        mainwindow.h \
    jumpjump.h \
    QImageMatConvert/mat_qimage_convert.h \
    ImageWidget-Qt/ImageWidget.h \
    ImageWidget-Qt/selectrect.h \
    jumpcontroller.h \
    fixparameterdialog.h

FORMS += \
        mainwindow.ui \
    fixparameterdialog.ui

# MinGW32

INCLUDEPATH += D:/opencv3.4.1/opencv_use/include

LIBS += D:/opencv3.4.1/opencv_use/lib/libopencv_*.a

# MSVC

#INCLUDEPATH += D:/opencv3.4.1/opencv/build/include

#CONFIG(debug, debug|release) {
#LIBS += D:/opencv3.4.1/opencv/build/x86/vc14/lib/*341d.lib
#} else {
#LIBS += D:/opencv3.4.1/opencv/build/x86/vc14/lib/*341.lib
#}
