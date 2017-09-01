#include "refraction.h"

//refer to scratchapixel
Ray refraction(Ray inray, Intersection hitPoint, float ior)
{
    QVector4D pos = hitPoint.getPos();
    QVector4D refractdir(0,0,0,0);
    QVector4D I = inray.getDir();
    QVector4D N = hitPoint.getNormal();
    float cosI = CLAMPF(QVector4D::dotProduct(I,N));
    float etai = 1, etat = ior;
    QVector4D n = N;
    if (cosI < 0){
        // outside the surface, want cos positive
        cosI = - cosI;
    }
    else{
        // inside the surface, reverse normal and swap refraction indices
        n = -N;
        std::swap(etai, etat);
    }
    float eta = etai/etat;
    float k = 1 - eta*eta*(1-cosI*cosI); // k is cos(theta2)
    if (k < 0){
        Ray refray(pos, refractdir); // ?
        return refray;
    }
    else{
        refractdir = eta*I + (eta*cosI - sqrtf(k))*n;
        Ray refray(pos, refractdir);
        return refray;
    }
}

// calculate fresnel factor, proportion of reflection and refraction light
float fresnel(Ray inray, Intersection hitPoint, float ior)
{
    QVector4D I = inray.getDir();
    QVector4D N = hitPoint.getNormal();
    float kr;
    float cosI = CLAMPF(QVector4D::dotProduct(I,N));
    float etai = 1, etat = ior;
    if (cosI > 0){
        std::swap(etai, etat);
    }
    float sint = etai/etat*sqrtf(std::max(0.f, 1-cosI*cosI));
    if (sint >= 1){
        // Total internal reflection
        kr = 1;
        return kr;
    }
    else{
        float cosT = sqrtf(std::max(0.f, 1 - sint * sint));
        cosI = fabsf(cosI);
        float Rs = ((etat * cosI) - (etai * cosT)) / ((etat * cosI) + (etai * cosT));
        float Rp = ((etai * cosI) - (etat * cosT)) / ((etai * cosI) + (etat * cosT));
        kr = (Rs * Rs + Rp * Rp) / 2;
        return kr;
    }
}
