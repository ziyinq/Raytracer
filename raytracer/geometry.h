#ifndef GEOMETRY_H
#define GEOMETRY_H

#include "json_obj_loader.h"
#include "intersection.h"

#include <QMatrix4x4>
#include <QVector>
#include <QVector4D>

class Intersection;

class Geometry {
protected:
    geo_t type; // geometry type
    QString name; // geometry name
    QMatrix4x4 modelMatrix; // model matrix
    int material_id; // material id in materials vector
public:
    Geometry();
    Geometry(geometry_t geo);
    geo_t getType(); // get the type of the geometry
    QString getName();
    QMatrix4x4 getModelMatrix(); // get the model matrix of the geometry
    int getMaterialId();
    virtual Intersection intersect(Geometry *geo, Ray rLocal) = 0; //geo and rLocal are in the local space, while the returned intersection is in the world space
};

class Cube: public Geometry {
private:
    float xMin, xMax, yMin, yMax, zMin, zMax;
    QVector4D origin;
public:
    Cube(geometry_t geo);
    float getXMin(); // get the x min of the cube
    float getXMax();
    float getYMin();
    float getYMax();
    float getZMin();
    float getZMax();
    QVector4D getOrigin(); // get the origin of the cube
    Intersection intersect(Geometry *geo, Ray rLocal);
};

class Sphere: public Geometry {
private:
    QVector4D origin;
    float radius;
public:
    Sphere(geometry_t geo);
    QVector4D getOrigin(); // get the origin of the sphere
    float getRadius(); // get the radius of the sphere
    Intersection intersect(Geometry *geo, Ray rLocal);
};

class Triangle: public Geometry {
private:
    QVector<QVector4D> vertices;
public:
    Triangle(geometry_t geo);
    QVector<QVector4D> getVertics(); // get the vertices of the triangle
    QVector4D getNormal(); // get the normal of the plane of the triangle
    Intersection intersect(Geometry *geo, Ray rLocal);
};

class Square: public Geometry {
private:
    float xMin, xMax, yMin, yMax;
    QVector<QVector4D> vertices;
public:
    Square(geometry_t geo);
    QVector<QVector4D> getVertices(); // get the vertices of the triangle
    float getXMin();
    float getXMax();
    float getYMin();
    float getYMax();
    QVector4D getNormal(); // get the normal of the plane of the triangle
    Intersection intersect(Geometry *geo, Ray rLocal);
};

class Cylinder: public Geometry {
private:
    float zMin, zMax;
    float radius;
public:
    Cylinder(geometry_t geo);
    float getZMin();
    float getZMax();
    float getRadius();
    Intersection intersect(Geometry *geo, Ray rLocal);
};

class Cone: public Geometry {
private:
    float zMax;
public:
    Cone(geometry_t geo);
    float getZMax();
    Intersection intersect(Geometry *geo, Ray rLocal);
};

class Light: public Geometry {
private:
    QVector4D position;
public:
    Light(geometry_t geo);
    QVector4D getPosition();
    Intersection intersect(Geometry *geo, Ray rLocal);
};

/// convert the vector of geometry_t to the vector of Geometry
QVector<Geometry *> convertGeometry(const QVector<geometry_t> rawVec);
void freeGeometry(QVector<Geometry *> ptrVec); // free the pointer to the geometries

#endif // GEOMETRY_H
