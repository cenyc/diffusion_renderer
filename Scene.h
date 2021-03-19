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
    DR::ImgBuff3 *targetImgBuff = (DR::ImgBuff3*) malloc(sizeof(zero<DR::ImgBuff3>()));
    DR::ImgBuff3 *imgBuff = (DR::ImgBuff3*) malloc(sizeof(zero<DR::ImgBuff3>()));
    DR::RayBuff *rayBuff = (DR::RayBuff*) malloc(sizeof(zero<DR::RayBuff>()));
    DR::DiffArrayf3 lightColor;
    DR::DF loss = 0.0;
    // Scene(Shape<DR::F3X, DR::S3X> shape_, Camera cam_)
    Scene(Camera cam_, Shape<DR::F3X, DR::S3X> *shape_) {
        this->shape = shape_;
        this->cam = cam_;
        (*this->imgBuff) = zero<DR::ImgBuff3>();
        (*this->rayBuff) = zero<DR::RayBuff>();
        lightColor = DR::DiffArrayf3(0.8, 0.7, 0.3);
    }
    /**
     * @brief 计算由环境光经衰减后的光强贡献（目前只考虑平行光）
     * 
     * @param nearInter 
     * @param farInter 
     * @return float 
     */
    float getIri(DR::Point dir, float dist);

    float getIri(Ray ray);

    static float attenuation(float dist);

    /**
     * @brief 计算targetImgBuff与imgBuff的loss值
     * 
     */
    void computeLoss();

    float rayMarching(const HitRecord& hitrecord);
    /**
     * @brief 渲染场景
     * 
     */
    void rendering(DR::ImgBuff3* ptrImgBuff);

    /**
     * @brief 渲染场景
     * 
     */
    void renderingParallel(DR::ImgBuff3* ptrImgBuff);

    /**
     * @brief 生成灰度图像，来源于场景中imgBuff的数据
     * 
     * @param filename 图像保存路径
     */
    void saveGrayImg(string filename);

    /**
     * @brief 保存彩色图像
     * 
     * @param filename 
     */
    void saveColorImg(string filename, DR::ImgBuff3 *ptrImgBuff);

    /**
     * @brief Destroy the Scene object
     * 
     */
    ~Scene() {
        Utils::msg("Destroy the Scene object.");
        free(this->rayBuff);
        free(this->imgBuff);
        free(this->targetImgBuff);
    }
};


// ENOKI_STRUCT_SUPPORT(Scene, shape, c÷\am)


#endif