#include "mainwindow.h"
#include <QApplication>
#include "raytrace.h"

int main(int argc, char *argv[])
{
    //------------ this program is running with Ubuntu 16.04  --------------------
    // Note: to add additional json file, make sure add it into the /json folder
    // Thanks!
    //----------------------------------------------------------------------------
    double start = clock();
    QApplication a(argc, argv);

    /// Cornel Box P.S. We have add an extra texture on the floor for cornel_box.
    QImage image = raytracer("../json/cornell_box.json"); //reflection + refraction  // takes about 12 sec
//    QImage image = accraytracer("../json/cornell_box.json"); // reflection only  // takes about 8 sec faster due to no refraction, not because of optimization
//    QImage image = anti_aliasraytracer("../json/cornell_box.json"); // reflection + refraction + antialias  // takes about 25 sec

    /// all_shapes with texturemapp and normal mapping
//    QImage image = raytracer("../json/all_shapes.json");

    /// New primitives
//    QImage image = raytracer("../json/my_shapes.json"); // new primitives: cone and clinder

    //-------------------------------------------------------
    // Run the following 3 files seperatly to test the speed
    // (we tested on Thinkpad P50 with Ubuntu 16.04)
    //-------------------------------------------------------

    ///===The original method: raytracer() ============
//    QImage image = raytracer("../json/gourd.json");   // ~141 sec
//    QImage image = raytracer("../json/dodechedron.json"); //~ 7.39 sec
//    QImage image = raytracer("../json/teapot.json"); // ~460 sec

    ///===The optimized method: accraytracer() ========
//    QImage image = accraytracer("../json/gourd.json"); // ~39 sec
//    QImage image = accraytracer("../json/dodechedron.json"); // ~2.83 sec
//    QImage image = accraytracer("../json/teapot.json"); // ~85 sec

    QLabel label;
    label.setPixmap(QPixmap::fromImage(image));
    label.show();

    double end = clock();
    std::cout<<"This program takes: "<< (double(end - start)/CLOCKS_PER_SEC)<<" sec."<<std::endl;
    return a.exec();
}
