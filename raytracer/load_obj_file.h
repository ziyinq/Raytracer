#ifndef LOAD_OBJ_FILE_H
#define LOAD_OBJ_FILE_H

#include "tiny_obj_loader.h"
#include "json_obj_loader.h"

#include <QString>
#include <QVector>
#include <QVector4D>

typedef struct {
    int index[3];
} Face_t;

QVector<Face_t> indices2Faces(const tinyobj::shape_t &shape); // convenience function used in loadOBj(), used to transform indices to faces
QVector<QVector4D> positions2Vec4s(const tinyobj::shape_t &shape); // convenience function used in loadOBj(), used to transform positions to vector4D
int loadObj(QString filename, QVector<geometry_t> &triangles); // read in the .obj file and transfer it to a vector of triangles


#endif // LOAD_OBJ_FILE_H
