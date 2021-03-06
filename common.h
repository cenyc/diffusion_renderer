#ifndef __COMMON_H__
#define __COMMON_H__
#include <iostream>
#include <fstream>
#include <sstream>
#include <malloc.h>
#include <omp.h>
#include <enoki/array.h>
#include <enoki/cuda.h>
#include <enoki/autodiff.h>
#include <enoki/special.h>
#include <enoki/dynamic.h>
#include <enoki/matrix.h>
#include <enoki-thread/thread.h>
using namespace enoki;
using namespace std;

NAMESPACE_BEGIN(DR)

static constexpr int WIDTH = 256;
static constexpr int HEIGHT = 256;

static const int ID_WIDTH = 64;
static const int ID_HEIGHT = 96;
// 学习率
static const float LR = 0.001;

static const int SAMPLE_NUM = 150;
// 环境光强度
static const int ENV_INTSITY = 1;
static const int ALBEDO = 1;
using FloatC = CUDAArray<float>;
using Vector3fC = Packet<FloatC, 3>;
using DiffArrayf3C = Packet<DiffArray<FloatC>, 3>;
using Vertor3f = Array<float, 3>;
using Vector2f = Array<float, 2>;
using Float = Array<float>;
using FloatP = Packet<float>;
using SizeP = Packet<size_t>;
using Point = Array<float, 3>;
using Vector3P = Packet<float, 3>;
using Vector3P3 = Array<Vector3P, 3>;
using Vector3P3X = DynamicArray<Vector3P3>;
using FloatX = DynamicArray<FloatP>;
using SizeX = DynamicArray<SizeP>;
using VectorWfC = Array<FloatC, 512>;
using DF = DiffArray<float>;
static const Point ENV_LIGTH_DIR = Point(1, 0, 0);

using Matrix3f  = enoki::Matrix<float, 3>;

using ID3 = Array<size_t, 3>;
using Edge = Point;

using Index10 = Packet<uint32_t, 10>;
using Index50 = Packet<uint32_t, 50>;
using Index500 = Packet<uint32_t, 500>;
using Index2x500 = Array<Index500, 2>;

using ImgfC = Array<VectorWfC, 512>;
using F3X = Array<FloatX, 3>;
using S3X = Array<SizeX, 3>;

using ImgBuff = Array<Packet<DF, DR::HEIGHT>, DR::WIDTH>;
using RayBuff = Array<ImgBuff, 3>;

using ImgBuff3 = Array<ImgBuff, 3>;
using DiffArrayf3 = Array<DF, 3>;
using ID0 = Array<Array<Packet<float, ID_WIDTH>, ID_HEIGHT>, ID_WIDTH>;

using ID1 = Array<Array<Packet<Point, ID_WIDTH>, ID_HEIGHT>, ID_WIDTH>;

using Density = Array<Array<Packet<float, ID_WIDTH>, ID_HEIGHT>, ID_WIDTH>;
//using IdGrid = Array<Array<Packet<Point, ID_WIDTH>, ID_HEIGHT>, ID_WIDTH>;

template <typename Value_, size_t Size_>
struct VectorX : StaticArrayImpl<Value_, Size_, false,
                                         VectorX<Value_, Size_>> {

    /// Base class
    using Base = StaticArrayImpl<Value_, Size_, false,
                                  VectorX<Value_, Size_>>;

    /// Helper alias used to implement type promotion rules
    template <typename T> using ReplaceValue = VectorX<T, Size_>;

    /// Mask type associated with this custom type
    using MaskType = enoki::Mask<Value_, Size_>;

    ENOKI_ARRAY_IMPORT(Base, VectorX)
};
NAMESPACE_END(DR)
#endif