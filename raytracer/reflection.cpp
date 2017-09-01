#include "reflection.h"
#include "refraction.h"

Ray reflection(Intersection hitpoint, Ray r)
{
    // slides 277 page 7
    QVector4D rdir = r.getDir();
    QVector4D norm = hitpoint.getNormal();
    QVector4D pos = hitpoint.getPos();
    QVector4D reflectrdir = rdir - 2*norm*QVector4D::dotProduct(rdir, norm);
    Ray reflectray(pos, reflectrdir);
    return reflectray;
}

QColor accgetReflectionColor(Ray inray, Intersection hitPoint, QVector<Geometry *> ptrVec, QVector<material_t> materials, int depth,
                          QMap<QString,QImage> texturedic,QMap<QString,QImage> normaldic)
{
    float fc1 = 1;
    float fc2 = 1;

    if (depth > 2){
        return QColor(0,0,0);
    }
    else{
        Intersection reflecthitPoint;
        float reflecttmin = INFINITY;
        Ray freflectray = reflection(hitPoint, inray);
        Ray reflectray(freflectray.getOrigin() + 10e-4 * freflectray.getDir(), freflectray.getDir());
        for(int k = 0; k < ptrVec.size(); k++) {
            // load the Model matrix of each Geometry;
            QMatrix4x4 modelinvert(ptrVec[k]->getModelMatrix().inverted()); //square for all_shapes.json

            // Now produce a copy of Ray into the Local space to do the intersection test
            Ray reflectLocal = reflectray.getTransformedCopy(modelinvert);

            // Next do the intersection test
            Intersection reflectinter = ptrVec[k]->intersect(ptrVec[k],reflectLocal);
            if(reflectinter.get_t() < reflecttmin && reflectinter.get_t() > 10e-4){
                // assign a unique color to each Geometry
                //value = qRgb(std::abs(255*inter.getNormal().x()),std::abs(255*inter.getNormal().y()),std::abs(255*inter.getNormal().z()));
                ///tmin = inter.get_t();
                reflecthitPoint = reflectinter; /// Sun
                reflecttmin = reflecthitPoint.get_t(); /// Sun
            }
        }
        if (reflecttmin < 50){
            QVector3D vecbaseColor = materials[hitPoint.getGeo()->getMaterialId()].baseColor;
            QColor baseColor(255*vecbaseColor.x(),255*vecbaseColor.y(),255*vecbaseColor.z());

            QColor diffuseColor = getDiffuseColor(ptrVec, hitPoint, baseColor,false);
            //------------------------- texture/normal mapping -------------------------------------
            QRgb value;
            // if it has a texture mapping:
            if(materials[hitPoint.getGeo()->getMaterialId()].texture != "") {
                QImage texture = texturedic[materials[hitPoint.getGeo()->getMaterialId()].texture];
                value = textureMapping(hitPoint,texture);
                QColor texturedcolor(value);
                // if it has a norm map:
                if(materials[hitPoint.getGeo()->getMaterialId()].normalMap != ""){
                    QImage normap = normaldic[materials[hitPoint.getGeo()->getMaterialId()].normalMap];
                    QVector4D newnor;
                    newnor= normalMapping(hitPoint,normap);
                    hitPoint.setNormal(newnor);
                    if(hitPoint.getGeo()->getType() == GEO_SQUARE){
                    }
                }
                diffuseColor = getDiffuseColor(ptrVec, hitPoint, texturedcolor,false);
//                value = lColor.rgb();
                return diffuseColor;
            }

            if(!materials[hitPoint.getGeo()->getMaterialId()].name.contains("specular",Qt::CaseInsensitive)){
                return diffuseColor;
            }


            //------------------------- finish texture/normal mapping -------------------------------------

            //QColor diffuseColor(230,150,150);
            depth = depth+1;
            QColor reflectColor = accgetReflectionColor(reflectray, reflecthitPoint, ptrVec, materials, depth,texturedic,normaldic);

            int iRed = fc2*diffuseColor.red() + fc1*reflectColor.red();
            int iGreen = fc2*diffuseColor.green() + fc1*reflectColor.green();
            int iBlue = fc2*diffuseColor.blue() + fc1*reflectColor.blue();
            QColor lColor(CLAMP(iRed), CLAMP(iGreen), CLAMP(iBlue));
            return lColor;
        }
        else{
            QVector3D vecbaseColor = materials[hitPoint.getGeo()->getMaterialId()].baseColor;
            QColor baseColor(255*vecbaseColor.x(),255*vecbaseColor.y(),255*vecbaseColor.z());
            QColor diffuseColor = getDiffuseColor(ptrVec, hitPoint, baseColor,false);
            //QColor diffuseColor(230,150,150);

            QRgb value;
            // if it has a texture mapping:
            if(materials[hitPoint.getGeo()->getMaterialId()].texture != "") {
                QImage texture = texturedic[materials[hitPoint.getGeo()->getMaterialId()].texture];
                value = textureMapping(hitPoint,texture);
                QColor texturedcolor(value);
                // if it has a norm map:
                if(materials[hitPoint.getGeo()->getMaterialId()].normalMap != ""){
                    QImage normap = normaldic[materials[hitPoint.getGeo()->getMaterialId()].normalMap];
                    QVector4D newnor;
                    newnor= normalMapping(hitPoint,normap);
                    hitPoint.setNormal(newnor);
                    if(hitPoint.getGeo()->getType() == GEO_SQUARE){
                    }
                }
                diffuseColor = getDiffuseColor(ptrVec, hitPoint, texturedcolor,false);
//                value = lColor.rgb();
//                return diffuseColor;
            }
            return diffuseColor;
        }
    }
}
QColor getReflectionColor(Ray inray, QVector<Geometry *> ptrVec, QVector<material_t> materials, int depth,
                          QMap<QString,QImage> texturedic,QMap<QString,QImage> normaldic)
{
    if (depth > 6){
        //beyond maximum depth
        return QColor(0,0,0);
    }
    else{
        // real hit point between the ray and the nearest geometry
        Intersection hitPoint(inray.getOrigin(), inray.getDir(), INFINITY, NULL);
        float tmin = INFINITY;

        for(int k = 0; k<ptrVec.size(); k++) {
            // load the Model matrix of each Geometry;
            QMatrix4x4 modelinvert(ptrVec[k]->getModelMatrix().inverted()); //square for all_shapes.json

            // Now produce a copy of Ray into the Local space to do the intersection test
            Ray rLocal = inray.getTransformedCopy(modelinvert);

            // Next do the intersection test
            Intersection inter = ptrVec[k]->intersect(ptrVec[k],rLocal);
            if(inter.get_t() < tmin && inter.get_t()>0){
                hitPoint = inter;
                tmin = hitPoint.get_t();
            }
        }
        if (tmin < 100){

            //------------------------- texture/normal mapping -------------------------------------
            // if it has a texture mapping:
            if(materials[hitPoint.getGeo()->getMaterialId()].texture != "") {
                QImage texture = texturedic[materials[hitPoint.getGeo()->getMaterialId()].texture];
                QRgb value = textureMapping(hitPoint,texture);
                QColor texturedcolor(value);
                // if it has a norm map:
                if(materials[hitPoint.getGeo()->getMaterialId()].normalMap != ""){
                    QImage normap = normaldic[materials[hitPoint.getGeo()->getMaterialId()].normalMap];
                    QVector4D newnor;
                    newnor= normalMapping(hitPoint,normap);
                    hitPoint.setNormal(newnor);
                    if(hitPoint.getGeo()->getType() == GEO_SQUARE){
                    }
                }
                QColor diffuseColor = getDiffuseColor(ptrVec, hitPoint, texturedcolor,true);
                //                value = lColor.rgb();
                return diffuseColor;
            }
            //---------------------- if it is diffuse -------------------------------------------
            if(materials[hitPoint.getGeo()->getMaterialId()].name.contains("diffuse",::Qt::CaseInsensitive)){
                QVector3D vecbaseColor = materials[hitPoint.getGeo()->getMaterialId()].baseColor;
                QColor baseColor(255*vecbaseColor.x(),255*vecbaseColor.y(),255*vecbaseColor.z());
                QColor diffuseColor = getDiffuseColor(ptrVec, hitPoint, baseColor,true);
                return diffuseColor;
            }
            //------------------------- if it is refractive ------------------------------------------------------
            if(materials[hitPoint.getGeo()->getMaterialId()].name.contains("refractive",::Qt::CaseInsensitive)) {
                float ior = 1.5;
                float kr = fresnel(inray, hitPoint, ior);

                QColor reflectColor(0,0,0);
                QColor refractColor(0,0,0);
                if (kr < 1){
                    // not total internal reflection
                    Ray frefractray = refraction(inray, hitPoint, ior);
                    Ray refractray(frefractray.getOrigin()+10e-5*frefractray.getDir(), frefractray.getDir());
                    refractColor = getReflectionColor(refractray, ptrVec, materials, depth+1,texturedic,normaldic);
                }
                Ray freflectray = reflection(hitPoint, inray);
                Ray reflectray(freflectray.getOrigin()+10e-5*freflectray.getDir(), freflectray.getDir());
                reflectColor = getReflectionColor(reflectray, ptrVec, materials, depth+1,texturedic,normaldic);
                int iRed = kr*reflectColor.red() + (1-kr)*refractColor.red();
                int iGreen = kr*reflectColor.green() + (1-kr)*refractColor.green();
                int iBlue = kr*reflectColor.blue() + (1-kr)*refractColor.blue();

                QColor lColor(CLAMP(iRed), CLAMP(iGreen), CLAMP(iBlue));
                return lColor;
            }
            //------------------------- if it is airbubble ------------------------------------------------------
            if(materials[hitPoint.getGeo()->getMaterialId()].name.contains("air",::Qt::CaseInsensitive)) {
                float ior = 1/1.5;
                float kr = fresnel(inray, hitPoint, ior);

                QColor reflectColor(0,0,0);
                QColor refractColor(0,0,0);
                if (kr < 1){
                    // not total internal reflection
                    Ray frefractray = refraction(inray, hitPoint, ior);
                    Ray refractray(frefractray.getOrigin()+10e-5*frefractray.getDir(), frefractray.getDir());
                    refractColor = getReflectionColor(refractray, ptrVec, materials, depth+1,texturedic,normaldic);
                }
                Ray freflectray = reflection(hitPoint, inray);
                Ray reflectray(freflectray.getOrigin()+10e-5*freflectray.getDir(), freflectray.getDir());
                reflectColor = getReflectionColor(reflectray, ptrVec, materials, depth+1,texturedic,normaldic);
                int iRed = kr*reflectColor.red() + (1-kr)*refractColor.red();
                int iGreen = kr*reflectColor.green() + (1-kr)*refractColor.green();
                int iBlue = kr*reflectColor.blue() + (1-kr)*refractColor.blue();

                QColor lColor(CLAMP(iRed), CLAMP(iGreen), CLAMP(iBlue));
                return lColor;
            }
            //------------------------- if it is mirror -----------------------------------------------------
            if(materials[hitPoint.getGeo()->getMaterialId()].name.contains("mirrored",::Qt::CaseInsensitive)) {
                QVector3D vecbaseColor = materials[hitPoint.getGeo()->getMaterialId()].baseColor;
                QColor baseColor(255*vecbaseColor.x(),255*vecbaseColor.y(),255*vecbaseColor.z());
                QColor diffuseColor = getDiffuseColor(ptrVec, hitPoint, baseColor,true);

                Ray freflectray = reflection(hitPoint, inray);
                Ray reflectray(freflectray.getOrigin()+10e-5*freflectray.getDir(), freflectray.getDir());
                QColor reflectColor = getReflectionColor(reflectray, ptrVec, materials, depth+1,texturedic,normaldic);
                int iRed = reflectColor.red() + diffuseColor.red();
                int iGreen = reflectColor.green() + diffuseColor.green();
                int iBlue = reflectColor.blue() + diffuseColor.blue();

                QColor lColor(CLAMP(iRed), CLAMP(iGreen), CLAMP(iBlue));
                return lColor;
            }
            //------------------------- if it is specular ---------------------------------------------------
            if(materials[hitPoint.getGeo()->getMaterialId()].name.contains("specular",::Qt::CaseInsensitive)) {
                QVector3D vecbaseColor = materials[hitPoint.getGeo()->getMaterialId()].baseColor;
                QColor baseColor(255*vecbaseColor.x(),255*vecbaseColor.y(),255*vecbaseColor.z());
                QColor diffuseColor = getDiffuseColor(ptrVec, hitPoint, baseColor,true);

                Ray freflectray = reflection(hitPoint, inray);
                Ray reflectray(freflectray.getOrigin()+10e-5*freflectray.getDir(), freflectray.getDir());
                QColor reflectColor = getReflectionColor(reflectray, ptrVec, materials, depth+1,texturedic,normaldic);
                int iRed = reflectColor.red() + diffuseColor.red();
                int iGreen = reflectColor.green() + diffuseColor.green();
                int iBlue = reflectColor.blue() + diffuseColor.blue();

                QColor lColor(CLAMP(iRed), CLAMP(iGreen), CLAMP(iBlue));
                return lColor;
            }
            // not match types above
            return QColor(0,0,0);
        }
        // no intersection
        else{
            return QColor(0,0,0);
        }
    }
}
