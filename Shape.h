#ifndef __SHAPE_H__
#define __SHAPE_H__
#include "common.h"
#include "Ray.h"
template <typename Ver, typename Tri> struct Shape
{
    /* data */
    int verNum;
    int faceNum;
    Ver ver;
    Tri tri;
    void intersect(Ray ray);

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
     * @param index_ 
     * @return DR::ID3 
     */
    DR::ID3 getTriIDs(size_t index_);

    ENOKI_STRUCT(Shape, verNum, faceNum, ver, tri)
};
ENOKI_STRUCT_SUPPORT(Shape, verNum, faceNum, ver, tri)

template<typename Ver, typename Tri>
DR::ID3 Shape<Ver, Tri>::getTriIDs(size_t index_) {
    return slice(this->tri, index_);
}

template<typename Ver, typename Tri>
DR::Point Shape<Ver, Tri>::getVer(size_t index_){
    return slice(this->ver, index_);
};



template<typename Ver, typename Tri>
void Shape<Ver, Tri>::intersect(Ray ray) {
    bool isHit = false;
    float dirT = 0.0, u = 0.0, v = 0.0;
    cout << this->tri << endl;
    for (size_t i = 0; i < this->faceNum; i++)
    {
        auto triIDs = getTriIDs(i);
        auto v0 = getVer(triIDs.x()),
        v1 = getVer(triIDs.y()),
        v2 = getVer(triIDs.z());
        // e1, e2
        DR::Edge e1 = v1 - v0, 
        e2 = v2 - v0;
        // cout << "e1 is " << e1 << ", e2 is " << e2 << endl;
        
    }
    
}

#endif