#include "lambert_shading.h"
#include <cmath>

QVector<Light *> getLights(QVector<Geometry *> ptrVec) {
    QVector<Light *> lightVec;
    // iterate through the ptrVec to find lights and append them to a light vector
    for(int k = 0; k < ptrVec.size(); k++) {
        if(ptrVec[k]->getType() == GEO_LIGTH)
            lightVec.append((Light *)ptrVec[k]);
    }
    return lightVec;
}


QColor getDiffuseColor(QVector<Geometry *> ptrVec, Intersection hitPoint, QColor baseColor,bool refrac) {
    if(hitPoint.get_t() == INFINITY) {
        QColor lColor(0, 0, 0);
        return lColor;
    }
    else {
        QColor lColor(0, 0, 0);
        // get a vector of light geometry
        QVector<Light *> lightVec = getLights(ptrVec);
        // for each light, compute the diffuse color for the hit point and then add them together
        for(int i = 0; i < lightVec.size(); i++) {
            // get the world position of the light
            QVector4D lightPosition = lightVec[i]->getModelMatrix() * lightVec[i]->getPosition();
            // get the direction vector from hit point to the light source
            QVector4D pHit = hitPoint.getPos();
            QVector4D lightDir = lightPosition - pHit;
            lightDir.normalize();
            // cast a shadow ray, we should move the pHit along ligthDir a little to avoid numerical problem
            Ray shadowRay(pHit + lightDir * 10e-4, lightDir);

            // t value for the light source, we can either use x, y or z to compute t value for the light source
            float tLight;
            // avoid numerical 0 division
            if(std::abs(lightDir.x()) > 10e-4)
                tLight = (lightPosition.x() - pHit.x()) / lightDir.x();
            else if(std::abs(lightDir.y()) > 10e-4)
                tLight = (lightPosition.y() - pHit.y()) / lightDir.y();
            else if(std::abs(lightDir.z()) > 10e-4)
                tLight = (lightPosition.z() - pHit.z()) / lightDir.z();

            bool isLit = true;
            for(int k = 0; k < ptrVec.size(); k++) {
                // load the Model matrix of each Geometry;
                QMatrix4x4 modelMatInv(ptrVec[k]->getModelMatrix().inverted());
                // Now produce a local space version shadow Ray to do the intersection test
                Ray rLocal = shadowRay.getTransformedCopy(modelMatInv);
                // Next do the intersection test
                Intersection inter = ptrVec[k]->intersect(ptrVec[k], rLocal);
                if (refrac && ptrVec[k]->getName().contains("transparent",Qt::CaseInsensitive)){
                    continue;
                }
                // if the t value for inter is less than the t value for the light and greater than 0, then hitPoint is in shadow
                if(inter.get_t() > 10e-4 && inter.get_t() < tLight) {
                    isLit = false;
                    break;
                }
            }
            // ambient constant
            QVector3D ambient(0.1, 0.1, 0.1);
//            QVector3D ambient(0.2, 0.2, 0.2);
            // diffusion constant
            QVector3D diff(0.8, 0.8, 0.8);
            // should use some kind of mixture
            QColor currentColor = LambertShade(hitPoint.getNormal(), lightDir, ambient, diff, isLit, baseColor);
            lColor.setRed(CLAMP(lColor.red() + currentColor.red()));
            lColor.setGreen(CLAMP(lColor.green() + currentColor.green()));
            lColor.setBlue(CLAMP(lColor.blue() + currentColor.blue()));
        }
        return lColor;
    }
}

QColor LambertShade(QVector4D norm, QVector4D l, QVector3D ambient, QVector3D diff, bool isLit, QColor baseColor) {
    // make sure that norm and l are normalized vector
    norm.normalize();
    l.normalize();
    // if the angle between norm and l is greater than 180 degree, flip the norm
    if(QVector4D::dotProduct(norm, l) < 0) {
        norm = -1 * norm;
    }
    int iRed = baseColor.red() * (ambient.x() + isLit * diff.x() * QVector4D::dotProduct(norm, l)) + 0.5;
    int iGreen = baseColor.green() * (ambient.y() + isLit * diff.y() * QVector4D::dotProduct(norm, l)) + 0.5;
    int iBlue = baseColor.blue() * (ambient.z() + isLit * diff.z() * QVector4D::dotProduct(norm, l)) + 0.5;
    QColor lColor(CLAMP(iRed), CLAMP(iGreen), CLAMP(iBlue));

    return lColor;
}
