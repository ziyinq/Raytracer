#include <cmath>
#include "linalg.h"
#include "camera.h"

QMatrix4x4 rotMat(QVector3D rotVec) {
    //rotVec = 1 * rotVec;
    QMatrix4x4 Rx(1, 0, 0, 0,
                  0, cos(rotVec.x() * PI / 180), -sin(rotVec.x() * PI / 180), 0,
                  0, sin(rotVec.x() * PI / 180), cos(rotVec.x() * PI / 180), 0,
                  0, 0, 0, 1);

    QMatrix4x4 Ry(cos(rotVec.y() * PI / 180), 0, sin(rotVec.y() * PI / 180), 0,
                  0, 1, 0, 0,
                  -sin(rotVec.y() * PI / 180), 0, cos(rotVec.y() * PI / 180), 0,
                  0, 0, 0, 1);

    QMatrix4x4 Rz(cos(rotVec.z() * PI / 180), -sin(rotVec.z() * PI / 180), 0, 0,
                  sin(rotVec.z() * PI / 180), cos(rotVec.z() * PI / 180), 0, 0,
                  0, 0, 1, 0,
                  0, 0, 0, 1);

   return Rx * Ry * Rz;
}

QMatrix4x4 transMat(QVector3D transVec) {
    QMatrix4x4 m(1, 0, 0, transVec.x(),
                 0, 1, 0, transVec.y(),
                 0, 0, 1, transVec.z(),
                 0, 0, 0, 1);
    return m;
}

QMatrix4x4 scaleMat(QVector3D scaleVec) {
    QMatrix4x4 m(scaleVec.x(), 0, 0, 0,
                 0, scaleVec.y(), 0, 0,
                 0, 0, scaleVec.z(), 0,
                 0, 0, 0, 1);
    return m;
}
