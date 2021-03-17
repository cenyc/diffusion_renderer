#ifndef __SHAPE_H__
#define __SHAPE_H__
#include "common.h"
#include "Ray.h"
#include <cmath>
#include <chrono>

#define pi 3.1415926

// #include "Utils.h"
struct HitTriangle {
    bool isHit = true;
    float dirT;
    float u, v;
};

static void setDensityValue(DR::Density * den, float _value);
static int64_t CurrentTimeMillis()
{
    int64_t timems = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    return timems;
}


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
    int sampleNum = 50;

    float sigmaA = 0.0;
    float sigmaS = 0.0;
    float sigmaT = 0.0;
    float hg_G   = 0.0;
    DR::Density *density = NULL;
    DR::Density *Qri_1 = NULL;
    DR::Density *kappa = NULL;

    /**
     * @brief 初始化
     * 
     */
    void setParam(float _sigmaA, float _sigmaS, float hg_G, DR::Density * _desity);
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
        free(this->density);
        free(this->Qri_1);
        free(this->kappa);
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
    /**
     * @brief phase function 输入cos_theta
     * 
     * @param cos_theta
     * @return 
     */
    float phaseFunction(float cos_theta);
    /**
     * @brief 输入n得到mu
     * 
     * @param n
     * @return 
     */
    float getMu(int n);
    /**
     * @brief 得到Sigma Tr
     * 
     * @param n
     * @return 
     */
    float getSigma_Tr();
    /**
     * @brief 得到Id0
     * 
     * @param light_intensity:光强 step：步长
     * @return 
     */
    DR::IdGrid * getGrid(DR::Density * _id0, float step);
    /**
     * @brief 得到Id0
     * 
     * @param light_intensity:光强 step：步长
     * @return 
     */
    DR::Density * getId0(float light_intensity, float step);
    /**
     * @brief 得到Id1
     * 
     * @param _id0:_id0 step：步长
     * @return 
     */
    DR::IdGrid * getId1(DR::Density * _id0, float step);
    /**
     * @brief 定义Id 0边界条件
     * 
     * @param id0,Qri_1,kappa, step：步长
     * @return void
     */
    void updataBoundary(DR::Density * _id0, DR::Density * _Qri_1, DR::Density * _kappa, float step);
    /**
     * @brief 计算Id 0内部值
     * 
     * @param id0,alpha,kappa, S, step：步长
     * @return void
     */
    void updataId0(DR::Density * _id0, DR::Density * _alpha, DR::Density * _kappa, DR::Density * _S, float step);
    /**
     * @brief 复制体数据的值
     * 
     * @param from,to
     * @return void
     */
    void copyValue(DR::Density * _from, DR::Density * _to);
    /**
     * @brief 计算松弛法迭代一次的损失
     * 
     * @param last， now
     * @return 损失
     */
    float countError(DR::Density * _lastValue, DR::Density * _nowValue);

    ENOKI_STRUCT(Shape, verNum, faceNum, ver, tri)
};
ENOKI_STRUCT_SUPPORT(Shape, verNum, faceNum, ver, tri)

#endif