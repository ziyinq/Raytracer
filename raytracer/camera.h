#ifndef CAMERA_H
#define CAMERA_H

#include <QVector4D>
#include <QVector3D>
#include <QMatrix4x4>
#include <cmath>
#include "ray.h"
#include "json_obj_loader.h"
#define PI 3.1415926545

class Camera
{
private:
    QVector3D eye;
    QVector3D worldup;
    QVector3D target;
    float fov;
    float width;
    float height;

public:
    Camera(camera_t cin);
    Camera(QVector3D eyein, QVector3D worldupin, QVector3D targetin, float fovin,
           float widthin, float heightin);
    float getWidth() const {return width;}
    float getHeight() const {return height;}
    float getFov() const {return fov;}
    QVector3D getEye() const {return eye;}
    void setFov(float nfov) {fov = nfov;}
    void setEye(QVector3D neye) {eye = neye;}
    QMatrix4x4 ProjMat(); // use the camera info to compute the corresponding ProjMat
    QMatrix4x4 viewMat(); // use the camera info to compute the corresponding viewMat
    Ray Raycast(float sx, float sy); //where sx and sy are screening coor
};

#endif // CAMERA_H
