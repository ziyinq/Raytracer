#ifndef RAYTRACE_H
#define RAYTRACE_H
#include <QVector4D>
#include <QVector3D>
#include <QApplication>
#include <QMatrix4x4>
#include "ray.h"
#include "reflection.h"
#include "intersection.h"
#include "camera.h"
#include <iostream>
#include <QDebug>
#include "geometry.h"
#include "maps.h"
#include "grid.h"
#include <QImage>
#include <QLabel>
#include <cmath>
#include "lambert_shading.h"

void PixeltoNDC(float width, float height, float &x, float &y);

/// get the t value for the intersection between the ray and the far clip
float tFarClip(Camera cam, Ray rWorld, float farClip);

QImage raytracer(QString filename);

QImage accraytracer(QString filename); // the optimized version
QImage anti_aliasraytracer(QString filename); // the antialias version


#endif // RAYTRACE_H
