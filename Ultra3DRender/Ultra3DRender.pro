#-------------------------------------------------
#
# Project created by QtCreator 2019-05-15T13:21:13
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

QT += opengl
TARGET = Ultra3DRender
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += main.cpp \
    commonControl.cpp \
    hoverpoints.cpp \
    light3d.cpp \
    shadewidget.cpp \
    tf.cpp \
    ultr3d.cpp \
    v3d.cpp \
    v3dException.cpp

HEADERS  += \
    commonControls.h \
    cu_tool.h \
    helper_cuda.h \
    helper_math.h \
    helper_string.h \
    hoverpoints.h \
    light3d.h \
    resource.h \
    shadewidget.h \
    tf.h \
    ui_v3d.h \
    ultr3d.h \
    v3d.h \
    v3dException.h

FORMS    += \
    v3d.ui
