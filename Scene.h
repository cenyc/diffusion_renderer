#ifndef __SCENE_H__
#define __SCENE_H__
#include "common.h"
#include "Shape.h"
#include "Camera.h"
#include "Utils.h"

struct Scene
{
    Shape<DR::F3X, DR::S3X> shape;
    Camera cam;
    
    DR::ImgBuff *imgBuff = (DR::ImgBuff*) malloc(sizeof(zero<DR::ImgBuff>()));
    DR::RayBuff *rayBuff = (DR::RayBuff*) malloc(sizeof(zero<DR::RayBuff>()));
    
    Scene(Shape<DR::F3X, DR::S3X> shape_, Camera cam_) {
        this->shape = shape_;
        this->cam = cam_;
        (*this->imgBuff) = zero<DR::ImgBuff>();
        (*this->rayBuff) = zero<DR::RayBuff>();
    }

    void rendering();
    void saveGrayImg(string filename, int * rgbData, int weight, int height);
    ~Scene() {
        Utils::msg("free scene's buff.");
        free(this->rayBuff);
        free(this->imgBuff);
    }
};


// ENOKI_STRUCT_SUPPORT(Scene, shape, c÷\am)


#endif