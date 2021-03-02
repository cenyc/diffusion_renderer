#ifndef __SHAPE_H__
#define __SHAPE_H__
#include "common.h"
#include "Ray.h"
// #include "Utils.h"
struct HitTriangle {
    bool isHit = true;
    float dirT;
    float u, v;
};

template <typename Ver, typename Tri> struct Shape
{
    /* data */
    int verNum;
    int faceNum;
    Ver ver;
    Tri tri;
    DR::ID0 *id0 = (DR::ID0*) malloc(sizeof(zero<DR::ID0>()));
    DR::ID1 *id1 = (DR::ID1*) malloc(sizeof(zero<DR::ID1>()));

    /**
     * @brief 对shape的顶点进行平移变换
     * 
     * @param tran （tran[0], tran[1], tran[2]）分别对应x, y, z轴的偏移量
     */
    void translate(DR::Vertor3f tran);

    /**
     * @brief 对shape的顶点进行尺度的变换
     * 
     * @param scale_ （scale_[0], scale_[1], scale_[2]）分别对应x, y, z轴方向缩放的尺度
     */
    void scale(DR::Vertor3f scale_);

    /**
     * @brief 根据光线和三角形的id索引计算该三角形与光线是否相交
     * 算法参考：https://www.cnblogs.com/graphics/archive/2010/08/09/1795348.html
     * @param ray 
     * @param faceID 
     * @return HitTriangle 
     */
    HitTriangle intersect(Ray ray, size_t faceID);

    /**
     * @brief 根据顶点ID获取顶点位置坐标
     * 
     * @tparam Ver 
     * @tparam Tri 
     * @param index_ 顶点ID
     * @return DR::Point 
     */
    DR::Point getVer(size_t index_);

    /**
     * @brief 根据面（三角形）索引ID获取所构成三角形的3顶点ID
     * 
     * @param faceID 
     * @return DR::ID3 
     */
    DR::ID3 getTriIDs(size_t faceID);

    ~Shape() {
        cout << "Free Shape space." << endl;
        free(this->id0);
        free(this->id1);
    }

    ENOKI_STRUCT(Shape, verNum, faceNum, ver, tri)
};
ENOKI_STRUCT_SUPPORT(Shape, verNum, faceNum, ver, tri)

template<typename Ver, typename Tri>
void Shape<Ver, Tri>::translate(DR::Vertor3f tran) {
    this->ver += tran;
}

template<typename Ver, typename Tri>
void Shape<Ver, Tri>::scale(DR::Vertor3f scale_) {
    DR::Matrix3f scaleTransform(scale_[0], 0.0f, 0.0f,
                                0.0f, scale_[1], 0.0f,
                                0.0f, 0.0f, scale_[2]);
    this->ver = scaleTransform*this->ver;
}

template<typename Ver, typename Tri>
DR::ID3 Shape<Ver, Tri>::getTriIDs(size_t faceID) {
    return slice(this->tri, faceID);
}

template<typename Ver, typename Tri>
DR::Point Shape<Ver, Tri>::getVer(size_t index_){
    return slice(this->ver, index_);
};

template<typename Ver, typename Tri>
HitTriangle Shape<Ver, Tri>::intersect(Ray ray, size_t faceID) {
    float dirT = 0.0, u = 0.0, v = 0.0;
    HitTriangle hitTri = HitTriangle();

    // 根据faceID的索引获取三个顶点的位置
    auto triIDs = getTriIDs(faceID);
    auto v0 = getVer(triIDs.x()),
    v1 = getVer(triIDs.y()),
    v2 = getVer(triIDs.z());

    // e1, e2
    DR::Edge e1 = v1 - v0, 
    e2 = v2 - v0;
    // p
    auto p = cross(ray.dir, e2);
    // determinant
    auto det = dot(e1, p);
    DR::Point t = zero<DR::Point>();

    if (det > 0)
    {
        t = ray.org - v0;
    }else {
        t = v0 - ray.org;
        det = -det;
    }

    if (det < 0.0001) {
        hitTri.isHit = false;
    }else {
        auto u = dot(t, p);
        if (u < 0 || u > det)
        {
            hitTri.isHit = false;
        }else {
            auto q = cross(t, e1);
            auto v = dot(ray.dir, q);
            if (v < 0 || (u+v) > det)   
            {
                hitTri.isHit = false;
            }else {
                float fInvDet = 1.0/det;
                hitTri.dirT = dot(e2, q) * fInvDet;
                hitTri.u = u * fInvDet;
                hitTri.v = v * fInvDet;
            }
        }
    }
    
    return hitTri;
}

#endif