#ifndef __RAY_H__
#define __RAY_H__
#include "common.h"

struct Ray {
    DR::Point org;
    DR::Point dir;

    Ray(DR::Point org_, DR::Point dir_): org(org_), dir(dir_) {}
};


#endif