#ifndef __SCENE_H__
#define __SCENE_H__
#include "common.h"
#include "Shape.h"
#include "Camera.h"
#include "Utils.h"

typedef struct Interaction
{
    DR::Point hitPoint;
    DR::Point dir;
    Interaction(DR::Point hitPoint_, DR::Point dir_): hitPoint(hitPoint_), dir(dir_) {}
}*PtrInteraction;

struct Scene
{
    Shape<DR::F3X, DR::S3X> *shape;
    Camera cam;
    
    DR::ImgBuff *imgBuff = (DR::ImgBuff*) malloc(sizeof(zero<DR::ImgBuff>()));
    DR::RayBuff *rayBuff = (DR::RayBuff*) malloc(sizeof(zero<DR::RayBuff>()));
    // Scene(Shape<DR::F3X, DR::S3X> shape_, Camera cam_)
    Scene(Camera cam_, Shape<DR::F3X, DR::S3X> *shape_) {
        this->shape = shape_;
        this->cam = cam_;
        (*this->imgBuff) = zero<DR::ImgBuff>();
        (*this->rayBuff) = zero<DR::RayBuff>();
    }
    float rayMarching(PtrInteraction nearPoint, PtrInteraction farPoint);
    /**
     * @brief 渲染场景
     * 
     */
    void rendering();

    /**
     * @brief 生成灰度图像，来源于场景中imgBuff的数据
     * 
     * @param filename 图像保存路径
     */
    void saveGrayImg(string filename);

    /**
     * @brief Destroy the Scene object
     * 
     */
    ~Scene() {
        Utils::msg("Destroy the Scene object.");
        free(this->rayBuff);
        free(this->imgBuff);
    }
};


// ENOKI_STRUCT_SUPPORT(Scene, shape, c÷\am)


#endif