#include "tiny_obj_loader.h"
#include "load_obj_file.h"
#include "json_obj_loader.h"

#include <iostream>
#include <QString>
#include <QVector>
#include <QVector4D>

using namespace std;

QVector<Face_t> indices2Faces(const tinyobj::shape_t &shape) {
    QVector<Face_t> fv;
    auto b = shape.mesh.indices.begin(); // iterator
    auto e = shape.mesh.indices.end(); // iterator
    for( ; b != e; b += 3) {
        Face_t f;
        f.index[0] = *b;
        f.index[1] = *(b + 1);
        f.index[2] = *(b + 2);
        fv.append(f);
    }
    return fv;
}

QVector<QVector4D> positions2Vec4s(const tinyobj::shape_t &shape) {
    QVector<QVector4D> vv;
    auto b = shape.mesh.positions.begin(); // iterator
    auto e = shape.mesh.positions.end(); // iterator
    for( ; b != e; b += 3) {
        QVector4D v;
        v[0] = *b;
        v[1] = *(b + 1);
        v[2] = *(b + 2);
        v[3] = 1;
        vv.append(v);
    }
    return vv;
}

int loadObj(QString filename, QVector<geometry_t> &triangles) {
    // vector passed to tinyobj::LoadObj(), storing shapes and materials parsed from the object file
    vector<tinyobj::shape_t> shapes;
    vector<tinyobj::material_t> materials;

    // read the object file
    string status;
    status = tinyobj::LoadObj(shapes, materials, filename.toLatin1().data());
    if(!status.empty()) {
        std::cout << "loadObj() failed to load the object!" << std::endl;
        return -1;
    }

    for(auto it = shapes.begin(); it != shapes.end(); ++it) {
        tinyobj::shape_t shape = (*it);
        // get the face array
        QVector<Face_t> fV = indices2Faces(shape);
        // get the vertex array
        QVector<QVector4D> vV = positions2Vec4s(shape);

        // iterate through the face array, to print each triangle
        for(auto b = fV.begin(); b != fV.end(); ++b) {
            geometry_t triangle;
            // get the three vertices of the triangle
            for(int i = 0; i < 3; ++i)
                triangle.v[i] = vV[(*b).index[i]];
            triangles.append(triangle);
        }
    }
    return 0;
}
