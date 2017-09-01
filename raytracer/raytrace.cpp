#ifndef RAYTRACE_CPP
#define RAYTRACE_CPP

#include <QDebug>
#include "raytrace.h"


void PixeltoNDC(float width,float height,float &x, float &y){
    x = (2*x/width) -1;
    y = 1- (2*y/height);
}


float tFarClip(Camera cam, Ray rWorld, float farClip) {
    // far center in the camera space
    QVector4D farCenter(0, 0, farClip, 1);
    // transform the far center to the world space
    QVector4D far = cam.viewMat().inverted() * farCenter;
    // get the norm of the far clip in the world space
    QVector4D norm = cam.getEye() - far;
    norm.normalize();
    float t = QVector4D::dotProduct(norm, (far - rWorld.getOrigin())) / QVector4D::dotProduct(norm, rWorld.getDir());
    return t;
}

QImage raytracer(QString filename)
{
    // ---------load json file --------------------------------
    camera_t camera; // camera to be filled
    QVector<geometry_t> geometries; // empty geometries vector to be filled
    QVector<material_t> materials; // empty materials vector to be filled
    LoadJson(camera, geometries, materials, filename);
    QVector<Geometry *> ptrVec = convertGeometry(geometries);

    Camera c1(camera);

    float width = c1.getWidth();
    float height = c1.getHeight();
    QImage image(width,height,QImage::Format_RGB32);

    //-----------------------------------------------------------
    // put Textures and Normal img into a QMap for latter access
    QMap<QString,QImage> texturedic;
    QMap<QString,QImage> normaldic;
    for(int k = 0; k<ptrVec.size(); k++){
        int idx = ptrVec[k]->getMaterialId();
        if(materials[idx].texture !=""){
            QImage texture("../json/" + materials[idx].texture);
            //Yuding: unforturnetly, my mapping calculation is backwards
            //so we transpose the input img as a shortcut debug solution.
            QTransform myTransform;
            myTransform.rotate(180);
            texture = texture.transformed(myTransform);
            texture = texture.mirrored(true,false);
            texturedic.insert(materials[idx].texture,texture);
        }

        if(materials[idx].normalMap !=""){
            QImage normap("../json/" +materials[idx].normalMap);
            //Yuding: unforturnetly, my mapping calculation is backwards
            //so we transpose the input img as a shortcut debug solution.
            QTransform myTransform;
            myTransform.rotate(180);
            normap = normap.transformed(myTransform);
            normap = normap.mirrored(true,false);
            normaldic.insert(materials[idx].normalMap,normap);
        }
    }

    //-----------start ray trace --------------------------------

    for(int j =0; j<c1.getHeight();++j){
        for(int i = 0; i<c1.getWidth();++i){
            float x = i;
            float y = j;
            // cast Primary ray
            PixeltoNDC(width, height,x, y);
            Ray r = c1.Raycast(x,y); // this Ray is in World space
            QColor finalcolor = getReflectionColor(r, ptrVec, materials, 1,texturedic,normaldic);
            image.setPixel(i ,j, finalcolor.rgb());

        }
//        qWarning()<<j;
    }

    freeGeometry(ptrVec);
    return image;
}

QImage anti_aliasraytracer(QString filename) // the antialias version
{
    // ---------load json file --------------------------------
    camera_t camera; // camera to be filled
    QVector<geometry_t> geometries; // empty geometries vector to be filled
    QVector<material_t> materials; // empty materials vector to be filled
    LoadJson(camera, geometries, materials, filename);
    QVector<Geometry *> ptrVec = convertGeometry(geometries);

    Camera c1(camera);

    float width = c1.getWidth();
    float height = c1.getHeight();
    QImage image(width,height,QImage::Format_RGB32);

    //-----------------------------------------------------------
    // put Textures and Normal img into a QMap for latter access
    QMap<QString,QImage> texturedic;
    QMap<QString,QImage> normaldic;
    for(int k = 0; k<ptrVec.size(); k++){
        int idx = ptrVec[k]->getMaterialId();
        if(materials[idx].texture !=""){
            QImage texture("../json/" + materials[idx].texture);
            //Yuding: unforturnetly, my mapping calculation is backwards
            //so we transpose the input img as a shortcut debug solution.
            QTransform myTransform;
            myTransform.rotate(180);
            texture = texture.transformed(myTransform);
            texture = texture.mirrored(true,false);
            texturedic.insert(materials[idx].texture,texture);
        }

        if(materials[idx].normalMap !=""){
            QImage normap("../json/" +materials[idx].normalMap);
            //Yuding: unforturnetly, my mapping calculation is backwards
            //so we transpose the input img as a shortcut debug solution.
            QTransform myTransform;
            myTransform.rotate(180);
            normap = normap.transformed(myTransform);
            normap = normap.mirrored(true,false);
            normaldic.insert(materials[idx].normalMap,normap);
        }
    }

    //-----------start ray trace --------------------------------
    // Anti-Aliasing included, implemented by randomly select 2 rays for each pixel
    for(int j =0; j<c1.getHeight();++j){
        for(int i = 0; i<c1.getWidth();++i){
            float x1 = i + ((float) rand() / (RAND_MAX));
            float y1 = j + ((float) rand() / (RAND_MAX));
            float x2 = i + ((float) rand() / (RAND_MAX));
            float y2 = j + ((float) rand() / (RAND_MAX));
            // cast Primary ray
            PixeltoNDC(width, height,x1, y1);
            PixeltoNDC(width, height,x2, y2);
            Ray r1 = c1.Raycast(x1,y1); // this Ray is in World space
            Ray r2 = c1.Raycast(x2,y2); // this Ray is in World space
            QColor finalcolor1 = getReflectionColor(r1, ptrVec, materials, 1,texturedic,normaldic);
            QColor finalcolor2 = getReflectionColor(r2, ptrVec, materials, 1,texturedic,normaldic);

            int red = 0.5*(finalcolor1.red() + finalcolor2.red());
            int green = 0.5*(finalcolor1.green() + finalcolor2.green());
            int blue = 0.5*(finalcolor1.blue() + finalcolor2.blue());

            QColor lColor(CLAMP(red), CLAMP(green), CLAMP(blue));
            image.setPixel(i ,j, lColor.rgb());

        }
//        qWarning()<<j;
    }

    freeGeometry(ptrVec);
    return image;
}

QImage accraytracer(QString filename)
{
    // ---------load json file --------------------------------
    camera_t camera; // camera to be filled
    QVector<geometry_t> geometries; // empty geometries vector to be filled
    QVector<material_t> materials; // empty materials vector to be filled
    LoadJson(camera, geometries, materials, filename);
    QVector<Geometry *> ptrVec = convertGeometry(geometries);

    Camera c1(camera);

    float width = c1.getWidth();
    float height = c1.getHeight();
    float farClip = 100;
    QImage image(width,height,QImage::Format_RGB32);

    //-----------------------------------------------------------
    // put Textures and Normal img into a QMap for latter access
    QMap<QString,QImage> texturedic;
    QMap<QString,QImage> normaldic;
    for(int k = 0; k<ptrVec.size(); k++){
        int idx = ptrVec[k]->getMaterialId();
        if(materials[idx].texture !=""){
            QImage texture("../json/" + materials[idx].texture);
            //Yuding: unforturnetly, my mapping calculation is backwards
            //so we transpose the input img as a shortcut debug solution.
            QTransform myTransform;
            myTransform.rotate(180);
            texture = texture.transformed(myTransform);
            texture = texture.mirrored(true,false);
            texturedic.insert(materials[idx].texture,texture);
        }

        if(materials[idx].normalMap !=""){
            QImage normap("../json/" +materials[idx].normalMap);
            //Yuding: unforturnetly, my mapping calculation is backwards
            //so we transpose the input img as a shortcut debug solution.
            QTransform myTransform;
            myTransform.rotate(180);
            normap = normap.transformed(myTransform);
            normap = normap.mirrored(true,false);
            normaldic.insert(materials[idx].normalMap,normap);
        }
    }

    //-----------start ray trace --------------------------------

    Grid g(filename);
    for(int j =0; j<c1.getHeight();++j){
        for(int i = 0; i<c1.getWidth();++i){
            float x = i;
            float y = j;
            // cast Primary ray
            PixeltoNDC(width, height,x, y);
            Ray r = c1.Raycast(x,y); // this Ray is in World space

            // real hit point between the ray and the nearest geometry
            Intersection hitPoint(r.getOrigin(), r.getDir(), INFINITY, NULL);
            float tmin = hitPoint.get_t();

            QVector <Cell> cellist = g.interray(r);
            if(cellist.size()!=0){
                for(int k = 0; k<cellist.size();k++){
//                    qWarning()<< cellist.size();
                    if(cellist[k].getGeoIn().size()!=0){
//                       qWarning()<< cellist[k].getGeoIn().size();
                        for(Geometry *geo : cellist[k].getGeoIn()){
                            // load the Model matrix of each Geometry;
                            QMatrix4x4 modelinvert(geo->getModelMatrix().inverted());

                            // Now produce a copy of Ray into the Local space to do the intersection test
                            Ray rLocal = r.getTransformedCopy(modelinvert);

                            // Next do the intersection test
                            Intersection inter = geo->intersect(geo,rLocal);
                            if(inter.get_t() < tmin){
                                hitPoint = inter;
                                tmin = hitPoint.get_t();
                            }
                        }
                    }
                }
            }

            if(tmin <= tFarClip(c1, r, farClip)){
                QColor finalcolor = accgetReflectionColor(r,hitPoint, ptrVec, materials, 1,texturedic,normaldic);

                image.setPixel(i ,j, finalcolor.rgb());
            }
        }
//        qWarning()<<j;
    }

    freeGeometry(ptrVec);
    return image;
}

#endif // RAYTRACE_CPP
