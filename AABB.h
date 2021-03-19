#ifndef __AABB_H__
#define __AABB_H__
#include "common.h"
#include "Ray.h"
struct HitRecord {
    bool isHit = false;
    float tMin = __FLT_MIN__;
    float tMax = __FLT_MAX__;
    float length;
    DR::Point dir;
    DR::Point p0;
    DR::Point p1;
};

struct AABB {
    DR::Point minimum;
    DR::Point maximum;

    DR::Point min;
    DR::Point max;
    DR::Point xyzLength;
    float step;

    void update();

    HitRecord hit(const Ray& ray);

    AABB(DR::Point minimun_, DR::Point maximum_): minimum(minimun_), maximum(maximum_) {
        this->update();
    }

};


#endif