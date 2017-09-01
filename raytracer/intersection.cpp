#include "intersection.h"


Intersection::Intersection(){
    wpoint = QVector4D(0,0,0,0);
    wnor = QVector4D(0,0,0,0);
    t = INFINITY;
    source = NULL;
}

Intersection::Intersection(QVector4D p, QVector4D n, float t_value, Geometry *s){
    wpoint = p;
    wnor = n;
    t = t_value;
    source = s;
}
float Intersection::get_t(){
    return t;
}


QVector4D Intersection::getPos(){
    return wpoint;
}
QVector4D Intersection::getNormal(){
    return wnor;
}
