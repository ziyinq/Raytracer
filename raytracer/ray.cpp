#include "ray.h"

Ray::Ray(QVector4D pos_in, QVector4D dir_in){
    pos = pos_in;
    dir = dir_in;
}

Ray::Ray(const Ray &r){
    pos = r.pos;
    dir = r.dir;
}


Ray Ray::getTransformedCopy(QMatrix4x4 transmat) const {
    QVector4D newpos = transmat*getOrigin();
    QVector4D newdir = transmat*getDir();
//    newdir.normalize();
    // should not normalize the newdir

    Ray newray(newpos, newdir);
    return newray;
}

QVector4D Ray::getOrigin() const{
    return pos;
}

QVector4D Ray::getDir() const{
    return dir;
}
