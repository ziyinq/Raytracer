#ifndef MAPS_H
#define MAPS_H
#include <QVector4D>
#include <QVector3D>
#include <QApplication>
#include <QMatrix4x4>
#include "ray.h"
#include "intersection.h"
#include "camera.h"
#include <iostream>
#include <QDebug>
#include "geometry.h"
#include <QImage>
#include <QLabel>
#include <cmath>

void Bilinearcalc(int &r, int &g, int &b, float uv_x, float uv_y, QImage texture);
QRgb textureMap_sq(QImage texture,Intersection inter);
QRgb textureMap_sp(QImage texture, Intersection inter);
QRgb textureMap_cb(QImage texture,Intersection inter);

QRgb textureMapping(Intersection inter, QImage texture);
QVector4D normalMapping(Intersection inter, QImage normalmap);

QVector4D normMap_sq(QImage normMap, Intersection inter);
QVector4D normMap_sp(QImage normMap, Intersection inter);
QVector4D normMap_cb(QImage normMap, Intersection inter);

#endif // MAPS_H
