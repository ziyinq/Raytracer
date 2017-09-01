#ifndef REFRACTION_H
#define REFRACTION_H
#define CLAMPF(x) ((x) > 1 ? 1 : ((x) < -1 ? -1 : (x)))

#include "intersection.h"
#include "ray.h"

Ray refraction(Ray inray, Intersection hitPoint, float ior);

float fresnel(Ray inray, Intersection hitPoint, float ior);
#endif // REFRACTION_H
