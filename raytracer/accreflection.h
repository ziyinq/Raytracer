#ifndef REFLECTION_H
#define REFLECTION_H

#include "intersection.h"
#include "ray.h"
#include "lambert_shading.h" /// Sun
#include "raytrace.h"
#include "maps.h"

#define CLAMP(x) ((x) > 255 ? 255 : ((x) < 0 ? 0 : (x)))

//Ray reflection(Intersection hitpoint, Ray r);

QColor accgetReflectionColor(Ray inray, Intersection hitPoint, QVector<Geometry *> ptrVec, QVector<material_t> materials, int depth,
                          QMap<QString,QImage> texturedic, QMap<QString,QImage> normaldic);

#endif // REFLECTION_H
