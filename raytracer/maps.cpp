#include "maps.h"

QRgb textureMapping(Intersection inter, QImage texture){
    Geometry *geo = inter.getGeo();
    QVector4D interpos = inter.getPos();
    QVector4D internor = inter.getNormal();
    //Need to conver this intersection from world to local
    interpos = geo->getModelMatrix().inverted()*interpos;
    internor = (geo->getModelMatrix().inverted().transposed()).inverted()*internor;
    internor.setW(0);
    // normalize the Normal vector
    internor.normalize();
    Intersection localinter(interpos,internor,inter.get_t(),geo);
    // now the intersection localinter is in geometry's local space
     
    QRgb outputcolor;
    switch (geo->getType()){
        case GEO_SQUARE:
            outputcolor = textureMap_sq(texture,localinter);
            break;
        case GEO_SPHERE:
            outputcolor = textureMap_sp(texture,localinter);
            break;
        case GEO_CUBE:
            outputcolor = textureMap_cb(texture,localinter);
            break;
        case GEO_TRIANGLE:
            outputcolor = textureMap_sq(texture,localinter);
            break;

    }
    return outputcolor;
}

QVector4D normalMapping(Intersection inter, QImage normalmap){
    Geometry *geo = inter.getGeo();

    QVector4D interpos = inter.getPos();
    QVector4D internor = inter.getNormal();
    //Need to conver this intersection from world to local
    interpos = geo->getModelMatrix().inverted()*interpos;
    internor = (geo->getModelMatrix().inverted().transposed()).inverted()*internor;
    internor.setW(0);
//     normalize the Normal vector
    internor.normalize();
    Intersection localinter(interpos,internor,inter.get_t(),geo);
    // now the intersection localinter is in geometry's local space

    QVector4D outputnorm;

    if(geo->getType() == GEO_SPHERE){
        outputnorm = normMap_sp(normalmap,localinter);
        outputnorm = geo->getModelMatrix().inverted().transposed()*outputnorm;
        outputnorm.setW(0);
        outputnorm.normalize();
        return outputnorm;
    }
    else if(geo->getType() == GEO_SQUARE){
         outputnorm = normMap_sq(normalmap,localinter);
         outputnorm =  geo->getModelMatrix().inverted().transposed()*outputnorm;
         outputnorm.setW(0);
         outputnorm.normalize();
         return outputnorm;
    }
    else if(geo->getType() == GEO_CUBE){
         outputnorm = normMap_cb(normalmap,localinter);
         outputnorm =  geo->getModelMatrix().inverted().transposed()*outputnorm;
         outputnorm.setW(0);
         outputnorm.normalize();
         return outputnorm;
    }
    else if(geo->getType() == GEO_TRIANGLE){
         outputnorm = normMap_sq(normalmap,localinter);
         outputnorm =  geo->getModelMatrix().inverted().transposed()*outputnorm;
         outputnorm.setW(0);
         outputnorm.normalize();
         return outputnorm;
    }

    return outputnorm;
}

// all is done in local space
QRgb textureMap_sq(QImage texture,Intersection inter){
    // Following slide of CIS 277 "3_22_Texture_and_Normal_Mapping.pdf";
    //the interpos is in square's local space
    QVector4D interpos = inter.getPos();

    // convert inperpos from local space to 'UV' coordinate
    // since we know for square, we should map from <-0.5, -0.5> to <0,0>
    // and <0.5,0.5> to <1,1> we just add 0.5 to interpos.x and interpos.y
    float uv_x = interpos.x() + 0.5;
    float uv_y = interpos.y() + 0.5;

    int r,g,b;
    r=g=b=0;

    /// Now perform a bilinear interpolation to compute the target color r,g,b
    /// and assign it to r,g,b;
    Bilinearcalc(r,g,b,uv_x,uv_y,texture);

    QRgb value = qRgb(r,g,b);
    return value;
}

QRgb textureMap_cb(QImage texture,Intersection inter){
    // attach the texture onto each faces of cube
    //the interpos is in square's local space
    QVector4D interpos = inter.getPos();

    // convert inperpos from local space to 'UV' coordinate
    // since we know for square, we should map from <-0.5, -0.5> to <0,0>
    // and <0.5,0.5> to <1,1> we just add 0.5 to interpos.x and interpos.y
    float uv_x = interpos.x() + 0.5;
    float uv_y = interpos.y() + 0.5;
    float uv_z = interpos.z() + 0.5;

    int r,g,b;    // front face
    int r1,g1,b1; // back face
    int r2,g2,b2; // left face
    int r3,g3,b3; // right face
    int r4,g4,b4; // up face
    int r5,g5,b5; // down face
    r=g=b=r1=r2=r3=r4=r5=g1=g2=g3=g4=g5=b1=b2=b3=b4=b5=0;

    /// Now perform a bilinear interpolation to compute the target color r,g,b
    /// and assign it to r,g,b;
    Bilinearcalc(r,g,b,uv_x,uv_y,texture); //front face
    Bilinearcalc(r1,g1,b1,uv_x,uv_y,texture); //back face
    Bilinearcalc(r2,g2,b2,uv_z,uv_y,texture); //left face
    Bilinearcalc(r3,g3,b3,uv_z,uv_y,texture); //right face
    Bilinearcalc(r4,g4,b4,uv_x,uv_z,texture); //up face
    Bilinearcalc(r5,g5,b5,uv_x,uv_z,texture); //down face

    QRgb value;
    if(inter.getNormal() == QVector4D(0,0,1,0)){
        //front face
        value = qRgb(r,g,b);
    }
    else if(inter.getNormal() == QVector4D(0,0,-1,0)){
        //back face
        value = qRgb(r1,g1,b1);
    }
    else if(inter.getNormal() == QVector4D(-1,0,0,0)){
        //left face
        value = qRgb(r2,g2,b2);
    }
    else if(inter.getNormal() == QVector4D(1,0,0,0)){
        //right face
        value = qRgb(r3,g3,b3);
    }
    else if(inter.getNormal() == QVector4D(0,1,0,0)){
        //up face
        value = qRgb(r4,g4,b4);
    }
    else if(inter.getNormal() == QVector4D(0,-1,0,0)){
        //up face
        value = qRgb(r5,g5,b5);
    }

    return value;
}

QRgb textureMap_sp(QImage texture, Intersection inter){
    // Following slide of CIS 277 "3_22_Texture_and_Normal_Mapping.pdf";
    // the internor is the surface normal of sphere at local space
    QVector4D internor = inter.getNormal();

    // Use Polar coordinate to map from intersection to texture's 'UV' coordinate
    // We know that in local space, the center of a sphere is located at <0,0,0> with r = 0.5
    // such that theta = acos(internor.y); and fi = atan2f(z,x);(if fi<0, fi = fi + 2*PI)
    // therefore uv_x = 1 - fi/(2*pi)
    //           uv_y = 1 - theta/fi

    float theta = acos(internor.y());
    if(theta == 0){
        theta = PI;
    }

    float fi = atan2f(internor.z(),internor.x());
    if(fi<0){
        // if fi is nagetive, add 2*PI
        fi = fi + 2*PI;
    }
    float uv_x = 1 - fi/(2*PI);
    float uv_y = 1 - theta/PI;

    int r,g,b;
    r=g=b=0;

    /// Now perform a bilinear interpolation to compute the target color r,g,b
    Bilinearcalc(r,g,b, uv_x,uv_y,texture);
    QRgb value = qRgb(r,g,b);
    return value;
}


QVector4D normMap_sq(QImage normMap, Intersection inter){
    QVector4D interpos = inter.getPos();

    float uv_x = interpos.x() + 0.5;
    float uv_y = interpos.y() + 0.5;

    int r,g,b;
    r=g=b=0;

    Bilinearcalc(r,g,b,uv_x,uv_y,normMap);

    float x =(float) r*2/255.0 - 1;
    float y = (float) g*2/255.0 - 1;
    float z = (float) b*2/255.0 - 1;

    QVector4D newnorm(x,y,z,0);
    newnorm.normalize();
    return newnorm;
}

QVector4D normMap_cb(QImage normMap, Intersection inter){
    QVector4D interpos = inter.getPos();

    float uv_x = interpos.x() + 0.5;
    float uv_y = interpos.y() + 0.5;
    float uv_z = interpos.z() + 0.5;

    int r,g,b;    // front face
    int r1,g1,b1; // back face
    int r2,g2,b2; // left face
    int r3,g3,b3; // right face
    int r4,g4,b4; // up face
    int r5,g5,b5; // down face
    r=g=b=r1=r2=r3=r4=r5=g1=g2=g3=g4=g5=b1=b2=b3=b4=b5=0;

    /// Now perform a bilinear interpolation to compute the target color r,g,b
    /// and assign it to r,g,b;
    Bilinearcalc(r,g,b,uv_x,uv_y,normMap); //front face
    Bilinearcalc(r1,g1,b1,uv_x,uv_y,normMap); //back face
    Bilinearcalc(r2,g2,b2,uv_z,uv_y,normMap); //left face
    Bilinearcalc(r3,g3,b3,uv_z,uv_y,normMap); //right face
    Bilinearcalc(r4,g4,b4,uv_x,uv_z,normMap); //up face
    Bilinearcalc(r5,g5,b5,uv_x,uv_z,normMap); //down face

    float x,y,z;
    if(inter.getNormal() == QVector4D(0,0,1,0)){
        //front face
        x =(float) r*2/255.0 - 1;
        y = (float) g*2/255.0 - 1;
        z = (float) b*2/255.0 - 1;
    }
    else if(inter.getNormal() == QVector4D(0,0,-1,0)){
        //back face
        x = (float) r1*2/255.0 - 1;
        y = (float) g1*2/255.0 - 1;
        z = (float) b1*2/255.0 - 1;
    }
    else if(inter.getNormal() == QVector4D(-1,0,0,0)){
        //left face
        x = (float) r2*2/255.0 - 1;
        y = (float) g2*2/255.0 - 1;
        z = (float) b2*2/255.0 - 1;
    }
    else if(inter.getNormal() == QVector4D(1,0,0,0)){
        //right face
        x = (float) r3*2/255.0 - 1;
        y = (float) g3*2/255.0 - 1;
        z = (float) b3*2/255.0 - 1;
    }
    else if(inter.getNormal() == QVector4D(0,1,0,0)){
        //up face
        x = (float) r4*2/255.0 - 1;
        y = (float) g4*2/255.0 - 1;
        z = (float) b4*2/255.0 - 1;
    }
    else if(inter.getNormal() == QVector4D(0,-1,0,0)){
        //up face
        x = (float) r5*2/255.0 - 1;
        y = (float) g5*2/255.0 - 1;
        z = (float) b5*2/255.0 - 1;
    }

    QVector4D newnorm(x,y,z,0);
    newnorm.normalize();
    return newnorm;
}

QVector4D normMap_sp(QImage normMap, Intersection inter){
    QVector4D internor = inter.getNormal();

    float theta = acos(internor.y());
    float fi = atan2f(internor.z(),internor.x());
    if(fi<0){
        // if fi is nagetive, add 2*PI
        fi = fi + 2*PI;
    }
    float uv_x = 1 - fi/(2*PI);
    float uv_y = 1 - theta/PI;

    int r,g,b;
    r=g=b=0;

    /// Now perform a bilinear interpolation to compute the target color r,g,b
    Bilinearcalc(r,g,b, uv_x,uv_y,normMap);
    float x =(float) r*2/255.0 - 1;
    float y = (float) g*2/255.0 - 1;
    float z = (float) b*2/255.0 - 1;

    QVector4D newnorm(x,y,z,0);
    newnorm.normalize();
    return newnorm;

}

void Bilinearcalc(int &r, int &g, int &b, float uv_x, float uv_y, QImage texture){
    // Perform a bilinear interpolation to compute the target r,g,b
    // T(a,b) = T[i + delta_x, j + delta_y]
    //        = (1-delta_x)(1-delta_y)*T(i,j) + delta_x*(1-delta_y)*T(i+1,j)
    //         + (1-delta_x)*delta_y*T(i,j+1) + delta_x*delta_y*T(i+1,j+1);

    // the corresponding texture_x and texture_y which would be at the left botton corner
    int texture_x = (int) (uv_x*texture.width());
    int texture_y = (int) (uv_y*texture.height());

    float delta_x =  uv_x*texture.width() - texture_x;

    float delta_y =  uv_y*texture.height()- texture_y;

    //assign rgb value to the four corners ABCD:
    // where A --> T(i,j)
    //       B --> T(i+1,j)
    //       C --> T(i,j+1)
    //       D --> T(i+1,j+1)
    int Ar,Ag,Ab, Br,Bg,Bb, Cr,Cg,Cb, Dr,Dg,Db;
    QRgb A,B,C,D;
    int ax,ay,bx,by,cx,cy,dx,dy;
    ax = texture_x - 1;
    bx = texture_x;
    cx = texture_x - 1;
    dx = texture_x;
    ay = texture_y - 1;
    by = texture_y - 1;
    cy = texture_y;
    dy = texture_y;
    // handling the bounds
    if(ax <=0){
        ax = 0;
    }
    if(ax >=texture.width()){
        ax = texture.width() -1;
    }
    if(bx <=0){
        bx = 0;
    }
    if(bx >=texture.width()){
        bx = texture.width() -1;
    }
    if(cx <=0){
        cx = 0;
    }
    if(cx >=texture.width()){
        cx = texture.width() -1;
    }
    if(dx <=0){
        dx = 0;
    }
    if(dx >=texture.width()){
        dx = texture.width() -1;
    }
    if(ay <=0){
        ay = 0;
    }
    if(ax >=texture.height()){
        ay = texture.height() -1;
    }
    if(by <=0){
        by = 0;
    }
    if(by >=texture.height()){
        by = texture.height() -1;
    }
    if(cy <=0){
        cy = 0;
    }
    if(cy >=texture.height()){
        cy = texture.height() -1;
    }
    if(dy <=0){
        dy = 0;
    }
    if(dy >=texture.height()){
        dy = texture.height() -1;
    }
    A = texture.pixel(ax ,ay);
    B = texture.pixel(bx ,by);
    C = texture.pixel(cx ,cy );
    D = texture.pixel(dx ,dy );


    Ar = qRed(A);
    Ag = qGreen(A);
    Ab = qBlue(A);

    Br = qRed(B);
    Bg = qGreen(B);
    Bb = qBlue(B);

    Cr = qRed(C);
    Cg = qGreen(C);
    Cb = qBlue(C);

    Dr = qRed(D);
    Dg = qGreen(D);
    Db = qBlue(D);

    //Last apply the above equation to calc rgb through bilinear interpolation
    r = (int)(
                Ar*(1-delta_x)*(1-delta_y) + Br*(delta_x)*(1-delta_y) + Cr*(delta_y)*(1-delta_x) + Dr*(delta_x*delta_y)
                );
    g = (int)(
                Ag*(1-delta_x)*(1-delta_y) + Bg*(delta_x)*(1-delta_y) + Cg*(delta_y)*(1-delta_x) + Dg*(delta_x*delta_y)
                );
    b = (int)(
                Ab*(1-delta_x)*(1-delta_y) + Bb*(delta_x)*(1-delta_y) + Cb*(delta_y)*(1-delta_x) + Db*(delta_x*delta_y)
                );
}
