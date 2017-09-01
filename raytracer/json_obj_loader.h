#ifndef JSON_OBJ_LOADER_H
#define JSON_OBJ_LOADER_H

#include <QString>
#include <QVector>
#include <QVector3D>
#include <QVector4D>

typedef struct {
    QVector3D target;
    QVector3D eye;
    QVector3D worldUp;
    float fov;
    int width;
    int height;
} camera_t;

// the type of the geometry, view light source as a special geometry
enum geo_t {GEO_SPHERE, GEO_CUBE, GEO_TRIANGLE, GEO_SQUARE, GEO_LIGTH, GEO_CYLINDER, GEO_CONE};

typedef struct {
    QVector3D translate;
    QVector3D rotate;
    QVector3D scale;
} transform_t;

typedef struct {
    geo_t type;
    QString name;
    transform_t transform;
    QVector4D v[3]; // three vertiecs of the triangle
    QString material;
    int material_id; // material id in materials vector
} geometry_t;

// the type of the material
enum mat_t {MAT_LAMBERT };

typedef struct {
    mat_t type;
    QString name;
    QVector3D baseColor;
    QString texture;
    QString normalMap;
    bool emissive;
} material_t;


/// Loads .json from a file.
/// 'camera' will be filled with camera parameters
/// 'geometries' will be filled with a vector of geometries
/// 'materials' will be filled with a vector of materials
/// Returns empty string when loading successfully.
QString LoadJson(camera_t &camera,       // [output]
                 QVector<geometry_t> &geometries, // [output]
                 QVector<material_t> &materials, // [output]
                 QString filename);

/// convenience function used in LoadJson, used to create the camera
void setCamera(camera_t &camera, // [output]
               const QJsonObject &cameraValueObj);

/// convenience function used in LoadJson, used to create a vector of materials
void setMaterials(QVector<material_t> &materials, // [output]
                  const QJsonArray &materialValueArray);

/// convenience function used in LoadJson, used to create a vector of geometries
void setGeometries(QVector<geometry_t> &geometries, // [output]
                   const QVector<material_t> &materials, const QJsonArray &geometryValueArray, QString dirname);

/// set the name, transform, material and material id of the geometry
void setGeometry(geometry_t &geometry, // [output]
                 const QVector<material_t> &materials, const QJsonObject &geometryValueObj);

#endif // JSON_OBJ_LOADER_H
