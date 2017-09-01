#-------------------------------------------------
#
# Project created by QtCreator 2017-04-09T10:16:54
#
#-------------------------------------------------

QT       += core gui
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = raytracer
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS
CONFIG += c++11
# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += main.cpp\
        mainwindow.cpp \
    json_obj_loader.cpp \
    tiny_obj_loader.cc \
    load_obj_file.cpp \
    geometry.cpp \
    linalg.cpp \
    raytrace.cpp \
    intersection.cpp \
    camera.cpp \
    ray.cpp \
    lambert_shading.cpp \
    reflection.cpp \
    grid.cpp \
    maps.cpp \
    refraction.cpp


HEADERS  += mainwindow.h \
    json_obj_loader.h \
    geometry.h \
    tiny_obj_loader.h \
    load_obj_file.h \
    linalg.h \
    raytrace.h \
    camera.h \
    intersection.h \
    ray.h \
    lambert_shading.h \
    reflection.h \
    grid.h \
    maps.h \
    refraction.h

FORMS    += mainwindow.ui

DISTFILES += \
    raytracer.pro.user
