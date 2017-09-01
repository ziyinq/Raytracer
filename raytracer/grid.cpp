#include "grid.h"

bool Cell::contains(QVector<Geometry *> geoVec){
    // not the true precise algorihm, but it works, basically treat
    // all geometry and the cell like a sphere and compare the
    // distance between origin to the sum of radius
    float cellRadius = 0.5*pow(pow(Sidelength.x(),2) + pow(Sidelength.y(),2) + pow(Sidelength.z(),2),0.5);
    QVector4D cellcenter(Loc.x()+0.5*Sidelength.x(),Loc.y()+0.5*Sidelength.y(),
                         Loc.z()+0.5*Sidelength.z(),1);
    bool contain = false;

    for(Geometry * geo: geoVec){
        if(geo->getType() ==GEO_CUBE){
            Cube *cube = (Cube *) geo;
            QVector4D point(cube->getXMax(), cube->getYMax(), cube->getZMax(), 1);
            QVector4D ori = cube->getOrigin();
            point = cube->getModelMatrix()*point;
            ori = cube->getModelMatrix()*ori;
            float georadius = ori.toVector3D().distanceToPoint(point.toVector3D());
            float dis = cellcenter.toVector3D().distanceToPoint(ori.toVector3D());
            if(dis < georadius+cellRadius){
                GeoIn.push_back(geo);
                contain = true;
            }
        }
        else if(geo->getType() == GEO_SPHERE){
            Sphere *sph = (Sphere *)geo;
            QVector4D ori = sph->getOrigin();
            QVector4D point(sph->getRadius()*sqrt(2),0,0,1);
            point = sph->getModelMatrix()*point;
            ori = sph->getModelMatrix()*ori;
            float georadius = ori.toVector3D().distanceToPoint(point.toVector3D());
            float dis = cellcenter.toVector3D().distanceToPoint(ori.toVector3D());
            if(dis < georadius+cellRadius){
                GeoIn.push_back(geo);
                contain = true;
            }
        }
        else if(geo->getType() == GEO_SQUARE){
            Square *square = (Square *) geo;
            QVector4D point = square->getVertices()[0];
            QVector4D ori(0,0,0,1);
            point = square->getModelMatrix()*point;
            ori = square->getModelMatrix()*ori;
            float georadius = ori.toVector3D().distanceToPoint(point.toVector3D());
            float dis = cellcenter.toVector3D().distanceToPoint(ori.toVector3D());
            if(dis < georadius+cellRadius){
                GeoIn.push_back(geo);
                contain = true;
            }
        }
        else if(geo->getType() == GEO_TRIANGLE){
            Triangle *triangle = (Triangle *) geo;
            QVector<QVector4D> vertices = triangle->getVertics();
            float ox,oy,oz;
            ox = 0.333*(vertices[0].x() + vertices[1].x()+ vertices[2].x());
            oy = 0.333*(vertices[0].y() + vertices[1].y()+ vertices[2].y());
            oz = 0.333*(vertices[0].z() + vertices[1].z()+ vertices[2].z());
            QVector4D ori(ox,oy,oz,1);

            float georadius= 0;
            for(int i = 0; i<vertices.size();i++){
                vertices[i] = triangle->getModelMatrix()*vertices[i];
                ori = triangle->getModelMatrix()*ori;
                float len = ori.toVector3D().distanceToPoint(vertices[i].toVector3D());
                if(georadius < len){
                    georadius = len;
                }
            }
            float dis = cellcenter.toVector3D().distanceToPoint(ori.toVector3D());
            if(dis < georadius+cellRadius){
                GeoIn.push_back(geo);
                contain = true;
            }
        }
    }
    return contain;
}


Grid::Grid(QString filename){
    // ---------load json file and parse the scene-------------
    camera_t camera; // camera to be filled
    QVector<geometry_t> geometries; // empty geometries vector to be filled
    QVector<material_t> materials; // empty materials vector to be filled
    LoadJson(camera, geometries, materials, filename);
    QVector<Geometry *> ptrVec = convertGeometry(geometries);

    Camera c1(camera);

    float width = c1.getWidth();
    float height = c1.getHeight();
    QImage image(width,height,QImage::Format_RGB32);
    //--------- determin the size of grid and its resolution----
    QVector4D scenemax(-INFINITY,-INFINITY,-INFINITY,1);
    QVector4D scenemin(INFINITY,INFINITY,INFINITY,1);
    for(Geometry * geo: ptrVec){
        if(geo->getType() ==GEO_CUBE){
            Cube *cube = (Cube *) geo;
            QVector4D point(cube->getXMax(), cube->getYMax(), cube->getZMax(), 1);
            QVector4D ori = cube->getOrigin();
            //transform the above 2 vectors into world space
            point = cube->getModelMatrix()*point;
            ori = cube->getModelMatrix()*ori;
            //find the max possible point on this cube regardless of its orientation
            float len = ori.toVector3D().distanceToPoint(point.toVector3D());
            float xmax,ymax,zmax,xmin,ymin,zmin;
            xmax = ori.x() + len;
            ymax = ori.y() + len;
            zmax = ori.z() + len;
            xmin = ori.x() - len;
            ymin = ori.y() - len;
            zmin = ori.z() - len;
            //update the scenemax and scenemin
           if(xmax > scenemax.x()){
                scenemax.setX(xmax);
            }
            if(ymax > scenemax.y()){
                scenemax.setY(ymax);
            }
            if(zmax > scenemax.z()){
                scenemax.setZ(zmax);
            }
            if(xmin < scenemin.x()){
                scenemin.setX(xmin);
            }
            if(ymin < scenemin.y()){
                scenemin.setY(ymin);
            }
            if(zmin < scenemin.z()){
                scenemin.setZ(zmin);
            }
        }
        else if(geo->getType() == GEO_SPHERE){
            Sphere *sph = (Sphere *)geo;
            QVector4D ori = sph->getOrigin();
            QVector4D point(sph->getRadius()*sqrt(2),0,0,1);
            //transform the above 2 vectors into world space
            point = sph->getModelMatrix()*point;
            ori = sph->getModelMatrix()*ori;
            //find the max possible point on this cube regardless of its orientation
            float len = ori.toVector3D().distanceToPoint(point.toVector3D());
            float xmax,ymax,zmax,xmin,ymin,zmin;
            xmax = ori.x() + len;
            ymax = ori.y() + len;
            zmax = ori.z() + len;
            xmin = ori.x() - len;
            ymin = ori.y() - len;
            zmin = ori.z() - len;
            //update the scenemax and scenemin
            if(xmax > scenemax.x()){
                scenemax.setX(xmax);
            }
            if(ymax > scenemax.y()){
                scenemax.setY(ymax);
            }
            if(zmax > scenemax.z()){
                scenemax.setZ(zmax);
            }
            if(xmin < scenemin.x()){
                scenemin.setX(xmin);
            }
            if(ymin < scenemin.y()){
                scenemin.setY(ymin);
            }
            if(zmin < scenemin.z()){
                scenemin.setZ(zmin);
            }
        }
        else if(geo->getType() == GEO_SQUARE){
            Square *square = (Square *) geo;
            QVector4D point = square->getVertices()[0];
            QVector4D ori(0,0,0,1);
            //transform the above 2 vectors into world space
            point = square->getModelMatrix()*point;
            ori = square->getModelMatrix()*ori;
            //find the max possible point on this cube regardless of its orientation
            float len = ori.toVector3D().distanceToPoint(point.toVector3D());
            float xmax,ymax,zmax,xmin,ymin,zmin;
            xmax = ori.x() + len;
            ymax = ori.y() + len;
            zmax = ori.z() + len;
            xmin = ori.x() - len;
            ymin = ori.y() - len;
            zmin = ori.z() - len;
            //update the scenemax and scenemin
            if(xmax > scenemax.x()){
                scenemax.setX(xmax);
            }
            if(ymax > scenemax.y()){
                scenemax.setY(ymax);
            }
            if(zmax > scenemax.z()){
                scenemax.setZ(zmax);
            }
            if(xmin < scenemin.x()){
                scenemin.setX(xmin);
            }
            if(ymin < scenemin.y()){
                scenemin.setY(ymin);
            }
            if(zmin < scenemin.z()){
                scenemin.setZ(zmin);
            }
        }
        else if(geo->getType() == GEO_TRIANGLE){
            Triangle *triangle = (Triangle *) geo;
            QVector<QVector4D> vertices = triangle->getVertics();

            for(int i = 0; i<vertices.size();i++){
                vertices[i] = triangle->getModelMatrix()*vertices[i];

                //update the scenemax and scenemin
                if(vertices[i].x()>scenemax.x()){
                    scenemax.setX(vertices[i].x());
                }
                if(vertices[i].y()>scenemax.y()){
                    scenemax.setY(vertices[i].y());
                }
                if(vertices[i].z()>scenemax.z()){
                    scenemax.setZ(vertices[i].z());
                }

                if(vertices[i].x()<scenemin.x()){
                    scenemin.setX(vertices[i].x());
                }
                if(vertices[i].y()<scenemin.y()){
                    scenemin.setY(vertices[i].y());
                }
                if(vertices[i].z()<scenemin.z()){
                    scenemin.setZ(vertices[i].z());
                }
            }
        }
    }
    scenebox = BBox(scenemax,scenemin);
    lambda = 7;
    int N = ptrVec.size(); // has to take out light source later;
    float V = std::abs(scenebox.getMax().x() - scenebox.getMin().x())*std::abs(scenebox.getMax().y() - scenebox.getMin().y())*
            std::abs(scenebox.getMax().z() - scenebox.getMin().z());

    Nx = 1*std::abs(scenebox.getMax().x() - scenebox.getMin().x())*pow(lambda*N/V,1.0/3);
    Ny = 1*std::abs(scenebox.getMax().y() - scenebox.getMin().y())*pow(lambda*N/V,1.0/3);
    Nz = 1*std::abs(scenebox.getMax().y() - scenebox.getMin().y())*pow(lambda*N/V,1.0/3);
//    qWarning()<<Nx<<"  "<<Ny<<"  "<<Nz;

    float dx,dy,dz;
    dx = std::abs(scenebox.getMax().x() - scenebox.getMin().x())/Nx;
    dy = std::abs(scenebox.getMax().y() - scenebox.getMin().y())/Ny;
    dz = std::abs(scenebox.getMax().z() - scenebox.getMin().z())/Nz;
    QVector3D side(dx,dy,dz);

    // Next create Nx X Ny X Nz cells inside this scenebox
    int size = Nx*Ny*Nz;
    cells = new Cell[size]; //allocate the memory

    // Last put the scene into the scenebox and assign Geo* into the Cells that
    // contains Geo.

    for(int i = 0; i<Nx;i++){
        for(int j = 0; j<Ny;j++){
            for(int k= 0; k<Nz;k++){
                QVector4D loc = mapidxtoWorldCoor(i,j,k);
                int idx = getIdx(i,j,k);
                cells[idx].setSidelength(side);
                cells[idx].setLoc(loc);
                cells[idx].contains(ptrVec);
            }
        }
    }
}

int Grid::getIdx(int x, int y, int z) const{
    return x + Nx*y + Nx*Ny*z;
}

Cell Grid::getCell(int x, int y ,int z) const{
    int idx = getIdx(x,y,z);
    return cells[idx];
}

QVector4D Grid::mapidxtoWorldCoor(int x,int y,int z){
    float dx,dy,dz;
    dx = std::abs(scenebox.getMax().x() - scenebox.getMin().x())/Nx;
    dy = std::abs(scenebox.getMax().y() - scenebox.getMin().y())/Ny;
    dz = std::abs(scenebox.getMax().z() - scenebox.getMin().z())/Nz;
    float wx,wy,wz;
    wx = scenebox.getMin().x() + x*dx;
    wy = scenebox.getMin().y() + y*dy;
    wz = scenebox.getMin().z() + z*dz;
    QVector4D result(wx,wy,wz,1);
    return result;
}

void Grid::mapWorldCoortoidx(QVector4D WCoor, int &x, int &y, int &z){
    float dx,dy,dz;
    dx = std::abs(scenebox.getMax().x() - scenebox.getMin().x())/Nx;
    dy = std::abs(scenebox.getMax().y() - scenebox.getMin().y())/Ny;
    dz = std::abs(scenebox.getMax().z() - scenebox.getMin().z())/Nz;
    float wx,wy,wz;
    wx = WCoor.x();
    wy = WCoor.y();
    wz = WCoor.z();
    x = floor((wx - scenebox.getMin().x())/dx);
    y = floor((wy - scenebox.getMin().y())/dy);
    z = floor((wz - scenebox.getMin().z())/dz);
    //handling edge case
    if(x<=0){
        x = 0;
    }
    if(y<=0){
        y = 0;
    }
    if(z<=0){
        z = 0;
    }
    if(x>=Nx-1){
        x = Nx-1;
    }
    if(y>=Ny-1){
        y = Ny-1;
    }
    if(z>=Nz-1){
        z = Nz-1;
    }

}


QVector<Cell> Grid::interray(Ray r){
    QVector<Cell> cellist;
    //------------------------------------
    // using "fake" DDA algorithm
    // first find where the ray intersects the grid
    float tNear = -INFINITY, tFar = INFINITY;
    QVector4D max(scenebox.getMax().x(),scenebox.getMax().y(), scenebox.getMax().z(), 0);
    QVector4D min(scenebox.getMin().x(),scenebox.getMin().y(), scenebox.getMin().z(), 0);
    QVector4D rpos = r.getOrigin();
    QVector4D rdir = r.getDir();
    float t0,t1;
    for(int i = 0; i < 3; i++) {
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
    if(tNear<tFar){
        QVector4D interpos = r.getOrigin() + tNear*r.getDir();
        QVector4D outpos = r.getOrigin() + tFar*r.getDir();

        int xin,yin,zin,xout,yout,zout;
        mapWorldCoortoidx(interpos,xin,yin,zin);
        mapWorldCoortoidx(outpos,xout,yout,zout);
        // find the corresponding cell:
        int step = 0.7*pow(pow(xin-xout,2)+pow(yin-yout,2)+pow(zin-zout,2),0.5);

        float tTemp = tNear;
        float tdelta = (tFar-tNear)/(step);
        int xtemp,ytemp,ztemp;
        int couter = 0;
        while(tTemp<tFar){
            QVector4D tempos = r.getOrigin() + tTemp*r.getDir();
            mapWorldCoortoidx(tempos,xtemp,ytemp,ztemp);
            Cell c = getCell(xtemp,ytemp,ztemp);
            if(c.getGeoIn().size()!=0){
                cellist.push_back(c);
            }
            couter++;
            tTemp += tdelta;
        }
    }

    return cellist;
}




