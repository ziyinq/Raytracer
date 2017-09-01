#include "json_obj_loader.h"
#include "load_obj_file.h"

#include <QByteArray>
#include <QDebug>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>

using namespace std;

QString LoadJson(camera_t &camera,       // [output]
                 QVector<geometry_t> &geometries, // [output]
                 QVector<material_t> &materials, // [output]
                 QString filename) {
    // Read in json file
    QFile jsonFile;
    jsonFile.setFileName(filename);
    jsonFile.open(QIODevice::ReadOnly | QIODevice::Text);
    QByteArray jsonData = jsonFile.readAll();
    jsonFile.close();

    QJsonDocument jsonDoc = QJsonDocument::fromJson(jsonData);
    QJsonObject rootObj = jsonDoc.object();

    QJsonValue rootValue = rootObj.value(QString("scene"));
    QJsonObject sceneValueObj = rootValue.toObject();
    // set the camera
    QJsonValue cameraValue = sceneValueObj.value(QString("camera"));
    QJsonObject cameraValueObj = cameraValue.toObject();
    setCamera(camera, cameraValueObj);

    // set the materials vector
    QJsonValue materialValue = sceneValueObj.value(QString("material"));
    QJsonArray materialValueArray = materialValue.toArray();
    setMaterials(materials, materialValueArray);

    // set the geometries vector
    QJsonValue geometryValue = sceneValueObj.value(QString("geometry"));
    QJsonArray geometryValueArray = geometryValue.toArray();
    QFileInfo dir(filename);
    QString dirname = dir.dir().absolutePath();
    setGeometries(geometries, materials, geometryValueArray, dirname);

    /*qWarning() << "width: " << camera.width << " height: " << camera.height;
    qWarning() << "target:\t" << camera.target;
    qWarning() << "eye:\t" << camera.eye;
    qWarning() << "worldUp:\t" << camera.worldUp;
    qWarning() << "fov:\t" << camera.fov;
    qWarning() << " ";

    for(int i = 0; i < materials.size(); i++) {
        qWarning() << "materials No. " << i;
        qWarning() << "type:\t" << materials[i].type;
        qWarning() << "name:\t" << materials[i].name;
        qWarning() << "baseColor:\t" << materials[i].baseColor;
        qWarning() << "texture:\t" << materials[i].texture;
        qWarning() << "normalMap:\t" << materials[i].normalMap;
        qWarning() << "emissive:\t" << materials[i].emissive;
        qWarning() << " ";
    }

    for(int i = 0; i < geometries.size(); i++) {
        qWarning() << "geometries No. " << i;
        qWarning() << "type:\t" << geometries[i].type;
        qWarning() << "name:\t" <<geometries[i].name;
        qWarning() << "translate:\t" <<geometries[i].transform.translate;
        qWarning() << "rotate:\t" <<geometries[i].transform.rotate;
        qWarning() << "scale:\t" <<geometries[i].transform.scale;
        qWarning() << "material:\t" <<geometries[i].material;
        qWarning() << "material_id:\t" <<geometries[i].material_id;
        qWarning() << "vertices:\t" <<geometries[i].v[0] << geometries[i].v[1] << geometries[i].v[2];
        qWarning() << " ";
    }*/

    return "";
}

void setCamera(camera_t &camera, const QJsonObject &cameraValueObj) {
    // set the target of the camera
    QJsonValue targetValue = cameraValueObj.value(QString("target"));
    QJsonArray targetArray = targetValue.toArray();
    for(int i = 0; i <= 2; i++)
        camera.target[i] = targetArray[i].toDouble();

    // set the eye of the camera
    QJsonValue eyeValue = cameraValueObj.value(QString("eye"));
    QJsonArray eyeArray = eyeValue.toArray();
    for(int i = 0; i <= 2; i++)
        camera.eye[i] = eyeArray[i].toDouble();

    // set the worldUp of the camera
    QJsonValue worldUpValue = cameraValueObj.value(QString("worldUp"));
    QJsonArray worldUpArray = worldUpValue.toArray();
    for(int i = 0; i <= 2; i++)
        camera.worldUp[i] = worldUpArray[i].toDouble();

    // set the fov of the camera
    QJsonValue fovValue = cameraValueObj.value(QString("fov"));
    camera.fov = fovValue.toDouble();

    // set the width and height for the camera
    QJsonValue widthValue = cameraValueObj.value(QString("width"));
    camera.width = widthValue.toInt();
    QJsonValue heightValue = cameraValueObj.value(QString("height"));
    camera.height = heightValue.toInt();
}


void setMaterials(QVector<material_t> &materials, const QJsonArray &materialValueArray) {
    foreach (const QJsonValue &materialValue, materialValueArray) {
        material_t material;
        QJsonObject materialValueObj = materialValue.toObject();

        // set the type of the material
        QJsonValue typeValue = materialValueObj.value(QString("type"));;
        if(typeValue.toString() == "lambert")
            material.type = MAT_LAMBERT;

        // set the name of the material
        QJsonValue nameValue = materialValueObj.value(QString("name"));
        material.name = nameValue.toString();

        // set the worldUp of the material
        QJsonValue baseColorValue = materialValueObj.value(QString("baseColor"));
        QJsonArray baseColorArray = baseColorValue.toArray();
        for(int i = 0; i <= 2; i++)
            material.baseColor[i] = baseColorArray[i].toDouble();

        // set the texture of the material
        QJsonValue textureValue = materialValueObj.value(QString("texture"));
        if(textureValue.isUndefined()) // set the texture "" if not defined
            material.texture = "";
        else
            material.texture = textureValue.toString();

        // set the normal map of the material
        QJsonValue normalMapValue = materialValueObj.value(QString("normalMap"));
        if(normalMapValue.isUndefined()) // set the normal map "" if not defined
            material.normalMap = "";
        else
            material.normalMap = normalMapValue.toString();

        // set the emissive of the material
        QJsonValue emissiveValue = materialValueObj.value(QString("emissive"));
        if(emissiveValue.isUndefined()) // set the emissive to false if not defined
            material.emissive = false;
        else {
            if(emissiveValue.toString() == "true")
                material.emissive = true;
            else
                material.emissive = false;
        }

        // append the material
        materials.append(material);
    }
}


void setGeometries(QVector<geometry_t> &geometries, const QVector<material_t> &materials, const QJsonArray &geometryValueArray, QString dirname) {
    foreach (const QJsonValue &geometryValue, geometryValueArray) {
        QJsonObject geometryValueObj = geometryValue.toObject();
        //geometry.filename = "";
        // set the type and the filename (if applicable) of the geometry
        QJsonValue typeValue = geometryValueObj.value(QString("type"));

        // if the type is obj, read triangles from .obj file
        if(typeValue.toString() == "obj") {
            QVector<geometry_t> triangles;
            QString basename = geometryValueObj.value(QString("filename")).toString();
            // get the absolute path of the .obj file
            QString filename = dirname + '/' + basename;
            loadObj(filename, triangles);
            // iterate throught the triangles
            for(auto it = triangles.begin(); it != triangles.end(); it++) {
                it->type = GEO_TRIANGLE;
                setGeometry((*it), materials, geometryValueObj);
                geometries.append(*it);
            }
        }
        // else if the type is others
        else if(typeValue.toString() == "sphere") {
            geometry_t sphere;
            sphere.type = GEO_SPHERE;
            setGeometry(sphere, materials, geometryValueObj);
            geometries.append(sphere);
        }
        else if(typeValue.toString() == "cube") {
            geometry_t cube;
            cube.type = GEO_CUBE;
            setGeometry(cube, materials, geometryValueObj);
            geometries.append(cube);
        }
        else if(typeValue.toString() == "square") {
            geometry_t square;
            square.type = GEO_SQUARE;
            setGeometry(square, materials, geometryValueObj);
            geometries.append(square);
        }
        else if(typeValue.toString() == "cylinder") {
            geometry_t cylinder;
            cylinder.type = GEO_CYLINDER;
            setGeometry(cylinder, materials, geometryValueObj);
            geometries.append(cylinder);
        }
        else if(typeValue.toString() == "cone") {
            geometry_t cone;
            cone.type = GEO_CONE;
            setGeometry(cone, materials, geometryValueObj);
            geometries.append(cone);
        }
    }
}

void setGeometry(geometry_t &geometry, const QVector<material_t> &materials, const QJsonObject &geometryValueObj) {
    // set the name of the geometry
    QJsonValue nameValue = geometryValueObj.value(QString("name"));
    geometry.name = nameValue.toString();

    // set the transform of the geometry
    QJsonValue transformValue = geometryValueObj.value(QString("transform"));
    QJsonObject transformValueObj = transformValue.toObject();

    // set the translate of the geometry
    QJsonValue translateValue = transformValueObj.value(QString("translate"));
    if(translateValue.isUndefined()) // set the translate vector to (0, 0, 0) if not defined
        geometry.transform.translate[0] = geometry.transform.translate[1] = geometry.transform.translate[2] = 0;
    else {
        QJsonArray translateArray = translateValue.toArray();
        for(int i = 0; i <= 2; i++)
            geometry.transform.translate[i] = translateArray[i].toDouble();
    }

    // set the rotate of the geometry
    QJsonValue rotateValue = transformValueObj.value(QString("rotate"));
    if(rotateValue.isUndefined()) // set the rotate vector to (0, 0, 0) if not defined
        geometry.transform.rotate[0] = geometry.transform.rotate[1] = geometry.transform.rotate[2] = 0;
    else {
        QJsonArray rotateArray = rotateValue.toArray();
        for(int i = 0; i <= 2; i++)
            geometry.transform.rotate[i] = rotateArray[i].toDouble();
    }

    // set the scale of the geometry
    QJsonValue scaleValue = transformValueObj.value(QString("scale"));
    if(scaleValue.isUndefined()) // set the scale vector to (1, 1, 1) if not defined
        geometry.transform.scale[0] = geometry.transform.scale[1] = geometry.transform.scale[2] = 1;
    else {
        QJsonArray scaleArray = scaleValue.toArray();
        for(int i = 0; i <= 2; i++)
            geometry.transform.scale[i] = scaleArray[i].toDouble();
    }

    // set the material name of the geometry
    QJsonValue materialValue = geometryValueObj.value(QString("material"));
    geometry.material = materialValue.toString();

    // set the material_id and treat light source as a point
    for(int i = 0; i < materials.size(); i++) {
        if(geometry.material == materials[i].name) {
            geometry.material_id = i;
            // Check if this material is emissive. If so, treat this geometry as a point light source
            if(materials[i].emissive == true)
                geometry.type = GEO_LIGTH;
        }
    }
}
