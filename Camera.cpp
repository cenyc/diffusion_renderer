#include "Camera.h"


Ray Camera::getRay(int x, int y) {
    DR::Point org_ = org;
    DR::Point dir_;
    dir_ = (this->leftBottomCorner+x*this->stepX*this->dir_h+y*this->stepY*this->dir_v) - this->org;
    cout << "dir = " << dir_ << endl;
    return Ray(org_, dir_);
}

