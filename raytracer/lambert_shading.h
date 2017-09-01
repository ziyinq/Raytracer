#ifndef LAMBERT_SHADING_H
#define LAMBERT_SHADING_H

#include "intersection.h"
#include "geometry.h"
#include "ray.h"
#include "raytrace.h"

#include <QColor>

#define CLAMP(x) ((x) > 255 ? 255 : ((x) < 0 ? 0 : (x)))

/// get the diffusion color using lambert shading
QColor getDiffuseColor(QVector<Geometry *> ptrVec, Intersection hitPoint, QColor baseColor, bool refrac);
/// get the vector of lights
QVector<Light *> getLights(QVector<Geometry *> ptrVec);
/// get the color using the Lambert shading
QColor LambertShade(QVector4D norm, // norm at the hit point
                    QVector4D l, // vector pointing from the hit point to the light source
                    QVector3D ambient, // ambient constants, e.g. 0.1
                    QVector3D diff, // diffusion constants, e.g. 0.8
                    bool isLit, // flag indicating whether this point is lit or not
                    QColor baseColor); // base color of the hit point


#endif // LAMBERT_SHADING_H
