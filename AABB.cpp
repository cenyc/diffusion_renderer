#include "AABB.h"

HitRecord AABB::hit(const Ray& ray) {
    HitRecord hitRecord;
    for (size_t i = 0; i < 3; i++)
    {
        auto invD = 1.0/ray.dir[i];
        auto t0 = (min[i] - ray.org[i]) * invD;
        auto t1 = (max[i] - ray.org[i]) * invD;
        if (invD < 0.0f)
            swap(t0, t1);

        hitRecord.tMin = t0 > hitRecord.tMin ? t0 : hitRecord.tMin;
        hitRecord.tMax = t1 < hitRecord.tMax ? t1 : hitRecord.tMax;
        
        if (hitRecord.tMax <= hitRecord.tMin)
            return hitRecord;
    }
    hitRecord.isHit = true;
    hitRecord.dir = ray.dir;
    hitRecord.p0 = ray.org+hitRecord.tMin*ray.dir;
    hitRecord.p1 = ray.org+hitRecord.tMax*ray.dir;
    hitRecord.length = hitRecord.tMax - hitRecord.tMin;
    return hitRecord;
}

void AABB::update() {
    this->max = enoki::max(this->minimum, this->maximum);
    this->min = enoki::min(this->minimum, this->maximum);
    this->step = norm(this->maximum - this->minimum) / DR::SAMPLE_NUM;
    this->xyzLength = enoki::abs(this->max - this->min);
}