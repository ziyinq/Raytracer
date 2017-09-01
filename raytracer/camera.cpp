#include "camera.h"

Camera::Camera(camera_t cin)
{
    eye = cin.eye;
    worldup = cin.worldUp;
    target = cin.target;
    fov = cin.fov;
    width = cin.width;
    height = cin.height;
}

Camera::Camera(QVector3D eyein, QVector3D worldupin, QVector3D targetin, float fovin,
               float widthin, float heightin){
    eye = eyein;
    worldup = worldupin;
    target = targetin;
    fov = fovin;
    width = widthin;
    height = heightin;
}


QMatrix4x4 Camera::ProjMat(){
    float l,r,b,t,n,f,ar;
    f = 100;
    n = 0.1; //? not sure about this
    ar = width/height;

    t = n * tan(fov*PI/360);
    b = -n*tan(fov*PI/360);
    r = ar * t;
    l = ar * b;

    // ProjMat:
    /*       1/A*tanofhalf    0         (r+l)/(r-l)     0
     *       0          1/tanofhalf     (t+b)/(t-b)     0
     *       0                0             f/(f-n) -nf/(f-n)
     *       0                0              1          0
     *
     *   Equivalent to
     *         2n/(r-l)    0        (r+l)/(r-l)     0
     *         0        2n/(t-b)    (t+b)/(t-b)     0
     *         0           0             f/(f-n) -nf/(f-n)
     *         0           0              1          0
     *
     */
    QMatrix4x4 ProjMatrix( 2*n/(r-l),    0, (r+l)/(r-l),          0,
                           0,    2*n/(t-b), (t+b)/(t-b),          0,
                           0,            0,     f/(f-n), -n*f/(f-n),
                           0,            0,           1,          0);

    return ProjMatrix;
}


QMatrix4x4 Camera::viewMat(){
    // forward, up and right are all direction vector, so set w = 0;

    // forward = (target-eye).norm
    QVector3D forward = target-eye;
    forward.normalize();

    // right = forward x worldup
    QVector3D right = right.crossProduct(forward,worldup);
    right.normalize();

    // up = right x forward
    QVector3D up = up.crossProduct(right,forward);
    up.normalize();

    // viewMat:
    /*       Rx    Ry    Rz   0
     *       Ux    Uy    Uz   0
     *       Fx    Fy    Fz   0
     *       0     0     0    1
     */
    QMatrix4x4 viewOrientMatrix(right.x()  ,   right.y(),   right.z(), 0,
                          up.x()	 ,      up.y(),      up.z(), 0,
                          forward.x(), forward.y(), forward.z(), 0,
                          0          ,           0,           0, 1);

    QMatrix4x4 viewTranslateMatrix(1, 0, 0, -eye.x(),
                                   0, 1, 0, -eye.y(),
                                   0, 0, 1, -eye.z(),
                                   0, 0, 0, 1);

    return viewOrientMatrix * viewTranslateMatrix;
}

Ray Camera::Raycast(float sx, float sy){

    // we have assumed the screen is 1 unit lengh away from the eye
    // so z = 1; hence the point at screen as a world coor would be:
    //     P = viewMat^-1 *ProjecMat^-1 * ((nx,ny,1,1) * farClip)
    QMatrix4x4 viewMatinver = viewMat().inverted();
    QMatrix4x4 ProjecMatinver = ProjMat().inverted();
    float farClip = 100;

    QVector4D pndc(sx,sy,1,1);
    QVector4D P = viewMatinver*ProjecMatinver*(farClip*pndc);
    QVector4D eye4d = eye.toVector4D();
    eye4d.setW(1);
    QVector4D ray_ori = eye4d;
    QVector4D ray_dir = P-eye4d;

    ray_dir.setW(0); // dir is direction, thus w = 0;
    ray_dir.normalize();
    Ray generateray(ray_ori, ray_dir);

    return generateray;
}
