#include "geometry.h"
#include "linalg.h"

#include <QVector3D>

// Geometry class member
Geometry::Geometry() {
    type = GEO_SPHERE;
    name = "default";
    material_id = 0;
    modelMatrix = QMatrix4x4();
}

Geometry::Geometry(geometry_t geo) {
    type = geo.type;
    name = geo.name;
    material_id = geo.material_id;
    modelMatrix = transMat(geo.transform.translate) * rotMat(geo.transform.rotate) * scaleMat(geo.transform.scale);
}

geo_t Geometry::getType() {
    return type;
}

QString Geometry::getName() {
    return name;
}

QMatrix4x4 Geometry::getModelMatrix() {
    return modelMatrix;
}

int Geometry::getMaterialId() {
    return material_id;
}

// Cube class member
Cube::Cube(geometry_t geo): Geometry(geo) {
    xMin = yMin = zMin = -0.5;
    xMax = yMax = zMax = 0.5;
    origin = QVector4D(0, 0, 0, 1);
}

float Cube::getXMin() {
    return xMin;
}

float Cube::getXMax() {
    return xMax;
}

float Cube::getYMin() {
    return yMin;
}

float Cube::getYMax() {
    return yMax;
}

float Cube::getZMin() {
    return zMin;
}

float Cube::getZMax() {
    return zMax;
}

QVector4D Cube::getOrigin() {
    return origin;
}

Intersection Cube::intersect(Geometry *geo, Ray rLocal) {
    float tNear = -INFINITY, tFar = INFINITY;
    float t0, t1;
    // contain xMax, yMax, zMax
    QVector4D max(getXMax(), getYMax(), getZMax(), 0);
    // contain xMin, yMin, zMin
    QVector4D min(getXMin(), getYMin(), getZMin(), 0);
    QVector4D rpos = rLocal.getOrigin();
    QVector4D rdir = rLocal.getDir();
    for(int i = 0; i <= 2; i++) {
        // if the ray is parallel to the slab
        if(std::abs(rdir[i]) <= 1e-5) {
            // if rpos[i] < min[i] or rpos[i] > max[i], we miss
            if(rpos[i] < min[i] || rpos[i] > max[i]) {
                Intersection result(rpos, rdir, INFINITY, NULL);
                return result;
            }
            else
                continue;
        }
        else {
            t0 = (min[i] - rpos[i]) / rdir[i];
            t1 = (max[i] - rpos[i]) / rdir[i];
            // swap t0 and t1 if t0 is greater than t1
            if(t0 > t1) {
                float temp = t0;
                t0 = t1;
                t1 = temp;
            }
            if(t0 > tNear)
                tNear = t0;
            if(t1 < tFar)
                tFar = t1;
        }
    }
    if(tNear <= tFar) {
        QVector4D intersectionPos = rpos + rdir * tNear;
        QVector4D intersectionNorm;
        if(std::abs(intersectionPos.x()) >= std::abs(intersectionPos.y()) && std::abs(intersectionPos.x()) >= std::abs(intersectionPos.z())) {
            if(intersectionPos.x() > 0)
                intersectionNorm = QVector4D(1,0,0,0);
            else
                intersectionNorm = QVector4D(-1,0,0,0);
        }
        else if(std::abs(intersectionPos.y()) >=std::abs(intersectionPos.x()) && std::abs(intersectionPos.y()) >= std::abs(intersectionPos.z())) {
            if(intersectionPos.y() > 0)
                intersectionNorm = QVector4D(0,1,0,0);
            else
                intersectionNorm = QVector4D(0,-1,0,0);
        }
        else{
            if(intersectionPos.z() > 0)
                intersectionNorm = QVector4D(0,0,1,0);
            else
                intersectionNorm = QVector4D(0,0,-1,0);
        }
        // Last convert interpos and internorm from Local space back to World space
        intersectionPos = getModelMatrix() * intersectionPos;
        // norm /times (M^{-1})^{T}
        intersectionNorm = getModelMatrix().inverted().transposed() * intersectionNorm;
        Intersection result(intersectionPos, intersectionNorm, tNear, (Cube *)geo);
        return result;
    }
    else {
        Intersection result(rpos, rdir, INFINITY, NULL);
        return result;
    }
}

// Sphere class member
Sphere::Sphere(geometry_t geo): Geometry(geo) {
    origin = QVector4D(0, 0, 0, 1);
    radius = 0.5;
}

QVector4D Sphere::getOrigin() {
    return origin;
}

float Sphere::getRadius() {
    return radius;
}

Intersection Sphere::intersect(Geometry *geo, Ray rLocal) {
    float radius = getRadius();
    QVector4D pos = getOrigin();
    QVector4D rpos = rLocal.getOrigin();
    QVector4D rdir = rLocal.getDir();

    float a = rdir.x() * rdir.x() + rdir.y() * rdir.y() + rdir.z() * rdir.z();
    float b = 2 * ((rdir.x() * (rpos.x() - pos.x())) + rdir.y() * (rpos.y() - pos.y())
                 + rdir.z() * (rpos.z() - pos.z()));
    float c = pow((rpos.x() - pos.x()), 2) + pow((rpos.y() - pos.y()), 2) +
                    pow((rpos.z() - pos.z()), 2) - radius * radius;

    if(b * b > 4 * a * c) {
        float t0 = (-b - sqrt(b * b - 4 * a * c)) / (2 * a);
        float t1 = (-b + sqrt(b * b - 4 * a * c)) / (2 * a);
        if(t0 > 0) {
            // calculate the intersection position in local space
            QVector4D intersectionPos = rpos + t0 * rdir;
            // calculate the surface normal of the intersection
            QVector4D intersectionNorm = intersectionPos - pos;
            // convert intersectionPos from local space back to world space
            intersectionPos = getModelMatrix() * intersectionPos;
            // norm /times (M^{-1})^{T}
            intersectionNorm = getModelMatrix().inverted().transposed() * intersectionNorm;
            // normalize the surface normal
            intersectionNorm.setW(0);
            intersectionNorm.normalize();

            Intersection result(intersectionPos, intersectionNorm, t0, (Sphere *)geo);
            return result;
        }
        else if(t1 > 0) {
            // calculate the intersection position in local space
            QVector4D intersectionPos = rpos + t1 * rdir;
            // calculate the surface normal of the intersection
            QVector4D intersectionNorm = intersectionPos - pos;
            // convert intersectionPos from local space back to world space
            intersectionPos = getModelMatrix() * intersectionPos;
            // norm /times (M^{-1})^{T}
            intersectionNorm = getModelMatrix().inverted().transposed() * intersectionNorm;
            // normalize the surface normal
            intersectionNorm.setW(0);
            intersectionNorm.normalize();

            Intersection result(intersectionPos, intersectionNorm, t1, (Sphere *)geo);
            return result;
        }
        else {
            // t0 <= 0 and t1 <= 0
            Intersection result(rpos, rdir, INFINITY, NULL);
            return result;
        }
    }
    else {
        Intersection result(rpos, rdir, INFINITY, NULL);
        return result;
    }
}

// Triangle class member
Triangle::Triangle(geometry_t geo): Geometry(geo) {
    vertices.append(geo.v[0]);
    vertices.append(geo.v[1]);
    vertices.append(geo.v[2]);
}

QVector<QVector4D> Triangle::getVertics() {
    return vertices;
}

QVector4D Triangle::getNormal() {
    QVector3D vec1 = (vertices[1] - vertices[0]).toVector3D();
    QVector3D vec2 = (vertices[2] - vertices[0]).toVector3D();
    QVector3D norm = QVector3D::crossProduct(vec1, vec2).normalized();
    return norm;
}

Intersection Triangle::intersect(Geometry *geo, Ray rLocal) {
    QVector<QVector4D> vertices = getVertics();
    QVector4D norm = getNormal();
    QVector4D rpos = rLocal.getOrigin();
    QVector4D rdir = rLocal.getDir();
    float denom = QVector4D::dotProduct(norm, rdir);
    if(std::abs(denom) > 10e-6) {
        float t = QVector4D::dotProduct(norm, vertices[0] - rpos) / QVector4D::dotProduct(norm, rdir); //calculate t
        if (t < 0) {
            Intersection result(rpos, rdir, INFINITY, NULL);
            return result;
        }
        else {
            QVector4D E1 = vertices[1] - vertices[0];
            QVector4D E2 = vertices[2] - vertices[0];
            QVector4D T = rpos - vertices[0];
            QVector3D P = QVector3D::crossProduct(rdir.toVector3D(), E2.toVector3D());
            QVector3D Q = QVector3D::crossProduct(T.toVector3D(), E1.toVector3D());
            float factor = 1 / (QVector3D::dotProduct(P, E1.toVector3D()));
            float u = factor * QVector3D::dotProduct(P, T.toVector3D());
            float v = factor * QVector3D::dotProduct(Q, rdir.toVector3D());

            // condition for the point in triangle
            if (u >= 0 && v >= 0 && u + v <= 1) {
                // calc the intersection position in Local space;
                QVector4D intersectionPos = rpos + rdir * t;
                // calc the surface normal of this intersection:
                QVector4D intersectionNorm  = norm;
                // norm facing to camera
                if(QVector4D::dotProduct(intersectionNorm, rdir) > 0)
                    intersectionNorm = -1 * intersectionNorm;

                // convert interpos and intersnorm from Local space back to World space
                intersectionPos = getModelMatrix() * intersectionPos;
                // norm /times (M^{-1})^{T}
                intersectionNorm = getModelMatrix().inverted().transposed() * intersectionNorm;
                // normalize surface normal
                intersectionNorm.setW(0);
                intersectionNorm.normalize();

                Intersection result(intersectionPos, intersectionNorm, t, (Triangle *)geo);
                return result;
            }
            // the point is out of the triangle
            else {
                Intersection result(rpos, rdir, INFINITY, NULL);
                return result;
            }
        }
    }
    else {
        Intersection result(rpos, rdir, INFINITY, NULL);
        return result;
    }
}

// Square class member
Square::Square(geometry_t geo): Geometry(geo) {
    xMin = yMin = -0.5;
    xMax = yMax = 0.5;
    vertices.append(QVector4D(xMax, yMax, 0, 1));
    vertices.append(QVector4D(xMax, yMin, 0, 1));
    vertices.append(QVector4D(xMin, yMin, 0, 1));
    vertices.append(QVector4D(xMin, yMax, 0, 1));
}

QVector<QVector4D> Square::getVertices() {
    return vertices;
}

float Square::getXMin() {
    return xMin;
}

float Square::getXMax() {
    return xMax;
}

float Square::getYMin() {
    return yMin;
}

float Square::getYMax() {
    return yMax;
}

QVector4D Square::getNormal() {
    QVector3D vec1 = (vertices[1] - vertices[0]).toVector3D();
    QVector3D vec2 = (vertices[2] - vertices[0]).toVector3D();
    QVector3D norm = QVector3D::crossProduct(vec1, vec2).normalized();
    return norm;
}

Intersection Square::intersect(Geometry *geo, Ray rLocal) {
    QVector4D norm = getNormal();
    QVector<QVector4D> vertices = getVertices();
    QVector4D rpos = rLocal.getOrigin();
    QVector4D rdir = rLocal.getDir();

    float denom = QVector4D::dotProduct(norm,rdir);
    if (std::abs(denom) > 10e-6) {
        float t = QVector4D::dotProduct(norm, vertices[3] - rpos) / QVector4D::dotProduct(norm, rdir); //calculate t
        if(t < 0) {
            Intersection result(rpos, rdir, INFINITY, NULL);
            return result;
        }
        QVector4D intersectionPos = rpos + t * rdir; //intersect point coordinates

        if(intersectionPos.x() > getXMin() && intersectionPos.x() < getXMax() && intersectionPos.y() > getYMin() && intersectionPos.y() < getYMax()) {
            // calc the surface normal of this intersection:
            QVector4D intersectionNorm  = norm;
            // flip the norm if it has the same direction as ray
            if(QVector4D::dotProduct(intersectionNorm, rdir) >0)
                intersectionNorm = -1 * intersectionNorm;

            // Last convert interpos and intersnorm from Local space back to World space
            intersectionPos = getModelMatrix() * intersectionPos;
            // norm /times (M^{-1})^{T}
            intersectionNorm = getModelMatrix().inverted().transposed() * intersectionNorm;
            // normalize surface normal
            intersectionNorm.setW(0);
            intersectionNorm.normalize();

            Intersection result(intersectionPos, intersectionNorm, t, (Square *)geo);
            return result;
        }
        else {
            Intersection result(rpos,rdir, INFINITY, NULL);
            return result;
        }
    }
    else {
        Intersection result(rpos, rdir, INFINITY, NULL);
        return result;
    }
}

// Cylinder class member
Cylinder::Cylinder(geometry_t geo): Geometry(geo) {
    zMin = -0.5;
    zMax = 0.5;
    radius = 0.5;
}

float Cylinder::getZMin() {
    return zMin;
}

float Cylinder::getZMax() {
    return zMax;
}

float Cylinder::getRadius() {
    return radius;
}

Intersection Cylinder::intersect(Geometry *geo, Ray rLocal) {
    float radius = getRadius();
    float zMin = getZMin();
    float zMax = getZMax();
    QVector4D rpos = rLocal.getOrigin();
    QVector4D rdir = rLocal.getDir();

    // calculate the intersections for the top and bottom covers
    float tUp = INFINITY, tBottom = INFINITY;
    float tFace = INFINITY;
    QVector4D coverPos, coverNorm;
    if(std::abs(rdir.z()) >= 1e-4) {
        tBottom = (zMin - rpos.z()) / rdir.z();
        tUp = (zMax - rpos.z()) / rdir.z();
        float t0 = (tUp > tBottom) ? tBottom : tUp;
        float t1 = (tUp > tBottom) ? tUp : tBottom;
        if(t0 > 0) {
            coverPos = rpos + t0 * rdir;
            // check if the intersection of t0 is in the circle
            if(coverPos.x() * coverPos.x() + coverPos.y() * coverPos.y() < radius * radius)
                tFace = t0;
            else {
                coverPos = rpos + t1 * rdir;
                // check if the intersection of t1 is in the circle
                if(coverPos.x() * coverPos.x() + coverPos.y() * coverPos.y() < radius * radius)
                    tFace = t1;
                else
                    tFace = INFINITY;
            }
        }
        else if(t1 > 0) {
            coverPos = rpos + t1 * rdir;
            // check if the intersection of t0 is in circle
            if(coverPos.x() * coverPos.x() + coverPos.y() * coverPos.y() < radius * radius)
                tFace = t1;
            else
                tFace = INFINITY;
        }
        else
            tFace = INFINITY;
    }

    // calculate the intersections for the cylinder body
    float tBody = INFINITY;
    QVector4D bodyPos, bodyNorm;
    float a = rdir.x() * rdir.x() + rdir.y() * rdir.y();
    float b = 2 * (rdir.x() * rpos.x() + rdir.y() * rpos.y());
    float c = rpos.x() * rpos.x() + rpos.y() * rpos.y() - radius * radius;

    if(b * b > 4 * a * c) {
        float t0 = (-b - sqrt(b * b - 4 * a * c)) / (2 * a);
        float t1 = (-b + sqrt(b * b - 4 * a * c)) / (2 * a);
        if(t0 > 0) {
            // calculate the intersection position in local space
            bodyPos = rpos + t0 * rdir;
            // check if the intersection of t0 is in range
            if(bodyPos.z() >= zMin && bodyPos.z() <= zMax)
                tBody = t0;
            // the intersection of t0 is out of range
            else {
                bodyPos = rpos + t1 * rdir;
                // check if the intersection of t1 is in range
                if(bodyPos.z() >= zMin && bodyPos.z() <= zMax)
                    tBody = t1;
                else
                    tBody = INFINITY;
            }
        }
        else if(t1 > 0) {
            // calculate the intersection position in local space
            bodyPos = rpos + t1 * rdir;
            // check if the intersection of t1 is in range
            if(bodyPos.z() >= zMin && bodyPos.z() <= zMax)
                tBody = t1;
            else
                tBody = INFINITY;
        }
        else
            tBody = INFINITY;
    }
    else
        tBody = INFINITY;


    if(tFace == INFINITY && tBody == INFINITY) {
        Intersection result(rpos, rdir, INFINITY, NULL);
        return result;
    }
    else if(tFace <= tBody) {
        if(coverPos.z() >= 0)
            coverNorm = QVector4D(0, 0, 1, 0);
        else
            coverNorm = QVector4D(0, 0, -1, 0);
        // convert intersectionPos from local space back to world space
        coverPos = getModelMatrix() * coverPos;
        // norm /times (M^{-1})^{T}
        coverNorm = getModelMatrix().inverted().transposed() * coverNorm;
        // normalize the surface normal
        coverNorm.setW(0);
        coverNorm.normalize();

        Intersection result(coverPos, coverNorm, tFace, (Cylinder *)geo);
        return result;
    }
    else if(tFace > tBody) {
        // calculate the surface normal of the intersection
        bodyNorm.setX(bodyPos.x());
        bodyNorm.setY(bodyPos.y());
        bodyNorm.setZ(0);
        bodyNorm.setW(0);
        bodyNorm.normalize();
        // convert intersectionPos from local space back to world space
        bodyPos = getModelMatrix() * bodyPos;
        // norm /times (M^{-1})^{T}
        bodyNorm = getModelMatrix().inverted().transposed() * bodyNorm;
        // normalize the surface normal
        bodyNorm.setW(0);
        bodyNorm.normalize();

        Intersection result(bodyPos, bodyNorm, tBody, (Cylinder *)geo);
        return result;
    }
    Intersection result(rpos, rdir, INFINITY, NULL);
    return result;
}

// Cone class member
Cone::Cone(geometry_t geo): Geometry(geo) {
    zMax = 0.5;
}

float Cone::getZMax() {
    return zMax;
}

Intersection Cone::intersect(Geometry *geo, Ray rLocal) {
    float zMax = getZMax();
    QVector4D rpos = rLocal.getOrigin();
    QVector4D rdir = rLocal.getDir();

    // calculate the intersections for the cover
    float tFace = INFINITY;
    QVector4D coverPos;
    QVector4D coverNorm(0, 0, 1, 0);
    float denom = QVector4D::dotProduct(coverNorm, rdir);
    if(std::abs(denom) > 1e-5) {
        float t0 = (zMax - rpos.z()) / rdir.z();
        if(t0 > 0) {
            coverPos = rpos + t0 * rdir;
            // check if the intersection of t0 is in the circle
            if(coverPos.x() * coverPos.x() + coverPos.y() * coverPos.y() <= zMax * zMax)
                tFace = t0;
            else
                tFace = INFINITY;
        }
        else
            tFace = INFINITY;
    }

    // calculate the intersections for the cone body
    float tBody = INFINITY;
    QVector4D bodyPos, bodyNorm;
    float a = rdir.x() * rdir.x() + rdir.y() * rdir.y() - rdir.z() * rdir.z();
    float b = 2 * (rdir.x() * rpos.x() + rdir.y() * rpos.y() - rdir.z() * rpos.z());
    float c = rpos.x() * rpos.x() + rpos.y() * rpos.y() - rpos.z() * rpos.z();

    if(b * b > 4 * a * c) {
        float t0 = (-b - sqrt(b * b - 4 * a * c)) / (2 * a);
        float t1 = (-b + sqrt(b * b - 4 * a * c)) / (2 * a);
        if(t0 > 0) {
            // calculate the intersection position in local space
            bodyPos = rpos + t0 * rdir;
            // check if the intersection of t0 is in range
            if(bodyPos.z() >= 1e-4 && bodyPos.z() <= zMax)
                tBody = t0;
            // the intersection of t0 is out of range
            else {
                bodyPos = rpos + t1 * rdir;
                // check if the intersection of t1 is in range
                if(bodyPos.z() >= 1e-4 && bodyPos.z() <= zMax)
                    tBody = t1;
                else
                    tBody = INFINITY;
            }
        }
        else if(t1 > 0) {
            // calculate the intersection position in local space
            bodyPos = rpos + t1 * rdir;
            // check if the intersection of t1 is in range
            if(bodyPos.z() >= 1e-4 && bodyPos.z() <= zMax)
                tBody = t1;
            else
                tBody = INFINITY;
        }
        else
            tBody = INFINITY;
    }
    else
        tBody = INFINITY;


    if(tFace == INFINITY && tBody == INFINITY) {
        Intersection result(rpos, rdir, INFINITY, NULL);
        return result;
    }
    else if(tFace <= tBody) {
        // convert intersectionPos from local space back to world space
        coverPos = getModelMatrix() * coverPos;
        // norm /times (M^{-1})^{T}
        coverNorm = getModelMatrix().inverted().transposed() * coverNorm;
        // normalize the surface normal
        coverNorm.setW(0);
        coverNorm.normalize();

        Intersection result(coverPos, coverNorm, tFace, (Cone *)geo);
        return result;
    }
    else if(tFace > tBody) {
        // calculate the surface normal of the intersection
        bodyNorm.setX(bodyPos.x() / sqrt(bodyPos.x() * bodyPos.x() + bodyPos.y() * bodyPos.y()));
        bodyNorm.setY(bodyPos.y() / sqrt(bodyPos.x() * bodyPos.x() + bodyPos.y() * bodyPos.y()));
        bodyNorm.setZ(-1);
        bodyNorm.setW(0);
        bodyNorm.normalize();
        // convert intersectionPos from local space back to world space
        bodyPos = getModelMatrix() * bodyPos;
        // norm /times (M^{-1})^{T}
        bodyNorm = getModelMatrix().inverted().transposed() * bodyNorm;
        // normalize the surface normal
        bodyNorm.setW(0);
        bodyNorm.normalize();

        Intersection result(bodyPos, bodyNorm, tBody, (Cone *)geo);
        return result;
    }
    Intersection result(rpos, rdir, INFINITY, NULL);
    return result;
}


// Ligth class member
Light::Light(geometry_t geo): Geometry(geo) {
    position = QVector4D(0, 0, 0, 1);
}

QVector4D Light::getPosition() {
    return position;
}

Intersection Light::intersect(Geometry *geo, Ray rLocal) {
    Intersection result(rLocal.getOrigin(), rLocal.getDir(), INFINITY, NULL);
    return result;
}

QVector<Geometry *> convertGeometry(const QVector<geometry_t> rawVec) {
    QVector<Geometry *> ptrVec;
    for(int i = 0; i < rawVec.size(); i++) {
        geometry_t geometry = rawVec[i];
        if(geometry.type == GEO_SPHERE) {
            Geometry *ptr = new Sphere(geometry);
            ptrVec.append(ptr);
        }
        else if(geometry.type == GEO_CUBE) {
            Geometry *ptr = new Cube(geometry);
            ptrVec.append(ptr);
        }
        else if(geometry.type ==GEO_TRIANGLE) {
            Geometry *ptr = new Triangle(geometry);
            ptrVec.append(ptr);
        }
        else if(geometry.type == GEO_SQUARE) {
            Geometry *ptr = new Square(geometry);
            ptrVec.append(ptr);
        }
        else if(geometry.type == GEO_CYLINDER) {
            Geometry *ptr = new Cylinder(geometry);
            ptrVec.append(ptr);
        }
        else if(geometry.type == GEO_CONE) {
            Geometry *ptr = new Cone(geometry);
            ptrVec.append(ptr);
        }
        else if(geometry.type == GEO_LIGTH) {
            Geometry *ptr = new Light(geometry);
            ptrVec.append(ptr);
        }
    }
    return ptrVec;
}

void freeGeometry(QVector<Geometry *> ptrVec) {
    for(int i = 0; i < ptrVec.size(); i++)
        delete ptrVec[i];
}
