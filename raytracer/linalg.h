#ifndef LINALG_H
#define LINALG_H

#include <QMatrix4x4>
#include <QVector3D>


/// Creates a 3-D rotation matrix.
/// and outputs a 4x4 rotation matrix
QMatrix4x4 rotMat(QVector3D rotVec);

/// Takes an xyz displacement and outputs a 4x4 translation matrix
QMatrix4x4 transMat(QVector3D transVec);

/// Takes an xyz scale and outputs a 4x4 scale matrix
QMatrix4x4 scaleMat(QVector3D scaleVec);

#endif // LINALG_H
