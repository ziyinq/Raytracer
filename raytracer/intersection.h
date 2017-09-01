#ifndef INTERSECTION_H
#define INTERSECTION_H
#include "camera.h"
#include "ray.h"
#include <QVector4D>
#include <QVector3D>
#include <QMatrix4x4>
#include "geometry.h"

class Geometry;

class Intersection
{
private:
    QVector4D wpoint;
    QVector4D wnor;
    float t;
    Geometry *source;
public:
    Intersection();
    Intersection(QVector4D p, QVector4D n, float t_value, Geometry *s);
    float get_t();
    QVector4D getPos();
    QVector4D getNormal();
    Geometry * getGeo(){return source;}

    void setPos(QVector4D ipos){wpoint = ipos;}
    void setNormal(QVector4D inor){wnor = inor;}
    void set_t(float tin){t = tin;}
};

#endif // INTERSECTION_H
