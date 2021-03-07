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
    int orgVerID = 0;
    int diagonalVID[2] = {0, 6};
    float step = 0.0;
    int sampleNum = 25;

    /**
     * @brief 更新采样步长
     * 
     */
    void updateStepSize();

    /**
     * @brief 由世界坐标转换到体素坐标
     * 
     * @param ptrVer 
     * @return DR::Point 
     */
    DR::Point world2voxel(DR::Point* ptrVer);

    /**
     * @brief 由世界坐标转换到局部坐标
     * 
     * @param ptrVer 
     * @return DR::Point 
     */
    DR::Point world2local(DR::Point* ptrVer);

    /**
     * @brief 由局部坐标转换到体素坐标
     * 
     * @param ptrVer 
     * @return DR::Point 
     */
    DR::Point local2voxel(DR::Point* ptrVer);
    
    /**
     * @brief 获取体素标定原点
     * 
     * @return DR::Point 
     */
    DR::Point getOrgVer();

    /**
     * @brief 
     * minmax[0]-记录x、y、z轴的最小值
     * minmax[1]-记录x、y、z轴的最大值
     * minmax[2]-记录x、y、z轴的abs(minmax[1]-minmax[0])
     * 
     */
    DR::Vector3P3 minmax;

    /**
     * @brief 但顶点位置发生变化时，用来初始化或更新边界范围值。
     * 
     */
    void initMinmax();

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

    /**
     * @brief 根据体素坐标的位置获取id0的值
     * 
     * @param worldCoordinate 
     * @return float 
     */
    float getIntensity0(DR::Point voxelVer);

    /**
     * @brief 根据体素坐标的位置获取id1的值
     * 
     * @param voxelVer 
     * @return DR::Point 
     */
    DR::Point getIntensity1(DR::Point voxelVer);

    ENOKI_STRUCT(Shape, verNum, faceNum, ver, tri)
};
ENOKI_STRUCT_SUPPORT(Shape, verNum, faceNum, ver, tri)

template<typename Ver, typename Tri>
void Shape<Ver, Tri>::updateStepSize() {
    this->step = norm(this->getVer(this->diagonalVID[0]) - this->getVer(this->diagonalVID[1])) / this->sampleNum;
}

template<typename Ver, typename Tri>
DR::Point Shape<Ver, Tri>::getIntensity1(DR::Point voxelVer) {
    return (*this->id1)[voxelVer[0]][voxelVer[1]][voxelVer[2]];
}

template<typename Ver, typename Tri>
float Shape<Ver, Tri>::getIntensity0(DR::Point voxelVer) {
    return (*this->id0)[voxelVer[0]][voxelVer[1]][voxelVer[2]];
}

template<typename Ver, typename Tri>
DR::Point Shape<Ver, Tri>::getOrgVer() {
    return getVer(this->orgVerID);
}

template<typename Ver, typename Tri>
void Shape<Ver, Tri>::initMinmax () {
    for (size_t i = 0; i < 3; i++)
    {
        this->minmax[0][i] = hmin(this->ver[i]);
        this->minmax[1][i] = hmax(this->ver[i]);
        this->minmax[2][i] = abs(this->minmax[1][i] - this->minmax[0][i]);
    }
}

template<typename Ver, typename Tri>
DR::Point Shape<Ver, Tri>::world2voxel(DR::Point* ptrVer) {
    auto localVer = world2local(ptrVer);
    return local2voxel(&localVer);
}

template<typename Ver, typename Tri>
DR::Point Shape<Ver, Tri>::local2voxel(DR::Point* ptrVer) {
    return ((*ptrVer)/minmax[2]) * DR::Point(DR::ID_WIDTH-1, DR::ID_HEIGHT-1, DR::ID_WIDTH-1);
}

template<typename Ver, typename Tri>
DR::Point Shape<Ver, Tri>::world2local(DR::Point* ptrVer) {
    return (*ptrVer) - getOrgVer();
}

template<typename Ver, typename Tri>
void Shape<Ver, Tri>::translate(DR::Vertor3f tran) {
    this->ver += tran;
    this->initMinmax();
}

template<typename Ver, typename Tri>
void Shape<Ver, Tri>::scale(DR::Vertor3f scale_) {
    DR::Matrix3f scaleTransform(scale_[0], 0.0f, 0.0f,
                                0.0f, scale_[1], 0.0f,
                                0.0f, 0.0f, scale_[2]);
    this->ver = scaleTransform*this->ver;
    this->initMinmax();
    this->updateStepSize();
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