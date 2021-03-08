#include "Camera.h"


Ray Camera::getRay(float u, float v) {
    DR::Point org_ = org;
    DR::Point dir_;
    // float u = x+,
    // v = ;
    dir_ = (this->leftBottomCorner+u*this->dir_h+v*this->dir_v) - this->org;
    return Ray(org_, normalize(dir_));
}

