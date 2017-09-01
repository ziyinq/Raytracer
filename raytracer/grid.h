#ifndef GRID_H
#define GRID_H

#include <QDebug>
#include "raytrace.h"
#include "lambert_shading.h"
#include "reflection.h"
#include "maps.h"
#include "ray.h"


class BBox
{
private:
    QVector4D Max;
    QVector4D Min;
public:
    BBox(QVector4D max,QVector4D min){
        Max = max;
        Min =min;
    }
    BBox(){}
    QVector4D getMax(){return Max;}
    QVector4D getMin(){return Min;}
};

class Cell{
private:
    QVector<Geometry *> GeoIn;
    QVector4D Loc;
    QVector3D Sidelength; //(dx,dy,dz)
public:
    Cell(){}
    QVector4D getLoc() const {return Loc;}
    QVector<Geometry *> getGeoIn() const {return GeoIn;}
    QVector3D getSidelength(){return Sidelength;}
    void setLoc(QVector4D location){Loc = location;}
    void setGeo(QVector<Geometry *> geointer){GeoIn = geointer;}
    void setSidelength(QVector3D side){Sidelength = side;}
    bool contains(QVector<Geometry *> geoVec);
};

class Grid
{
private:
    int Nx, Ny, Nz;
    BBox scenebox;
    Cell * cells;
    double lambda; // the user defined resolution parameter; (we set it to be 5 by default);
public:
    // the constructer take scene as input and preprocess it
    Grid(QString filename);
    ~Grid(){delete [] cells;}
    int getIdx(int x, int y, int z)	const;
    Cell getCell(int x, int y ,int z) const;
    QVector4D mapidxtoWorldCoor(int x,int y,int z);
    void mapWorldCoortoidx(QVector4D WCoor, int &x, int &y, int &z);
    QVector<Cell> interray(Ray r);

};




#endif // GRID_H
