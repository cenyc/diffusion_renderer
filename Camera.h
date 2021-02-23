#ifndef __CAMERA_H__
#define __CAMERA_H__
#include "common.h"
#include "Ray.h"
struct Cone {
    float l = -1, r = 1, b = -1, t = 1, n = -1, f = -10;
};

struct Camera
{
    DR::Point org = zero<DR::Point>();
    DR::Point dir_h = DR::Point(1, 0, 0);
    DR::Point dir_v = DR::Point(0, 1, 0);
    int height = DR::HEIGHT;
    int width = DR::WIDTH;
    Cone cone;
    float stepX = abs(cone.l-cone.r)/width;
    float stepY = abs(cone.t-cone.b)/height;
    DR::Point leftBottomCorner = DR::Point(cone.l, cone.b, cone.n);
    Camera() {
        // dir.z() = -0.5;
    }
    Ray getRay(int x, int y);
    
};


#endif