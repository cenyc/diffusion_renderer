#include "Camera.h"


Ray Camera::getRay(int x, int y) {
    DR::Point org_ = org;
    DR::Point dir_;
    dir_ = (this->leftBottomCorner+x*this->stepX*this->dir_h+y*this->stepY*this->dir_v) - this->org;
    return Ray(org_, normalize(dir_));
}

