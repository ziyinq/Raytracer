#ifndef RAY_H
#define RAY_H
#include <QVector4D>
#include <QVector3D>
#include <QMatrix4x4>
class Ray
{
private:
    QVector4D pos; // the "origin", a point in space, So far, this is camera ray
                   // such that pos = eye

    QVector4D dir; // the direction, a normalized vector

public:
    Ray();
    Ray(const Ray &r); //copy constructor
    Ray(QVector4D pos_in, QVector4D dir_in);
    QVector4D getOrigin() const;
    QVector4D getDir() const;
    Ray getTransformedCopy(QMatrix4x4 transmat) const;
};

#endif // RAY_H
