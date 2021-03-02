#include "common.h"
#include "Utils.h"
#include "Shape.h"
#include "Camera.h"
#include "Scene.h"

using FloatC = CUDAArray<float>;
using FloatD = DiffArray<FloatC>;

template <typename Value> Value srgb_gamma(Value x) {
    return select(
        x <= 0.0031308f,
        x * 12.92f,
        pow(x * 1.055f, 1.f / 2.4f) - 0.055f
    );
}

void test() {

}

int main(int argc,char *argv[]){    
    // cuda_set_log_level(3);
    Shape<DR::F3X, DR::S3X> shape;
    Utils::readOFF("../data/cube.off", shape);

    // Shape<DR::F3X, DR::S3X> shape1;
    // cout << shape.tri << endl;
    // = Utils::readOFF("../data/cube.off");
    // Camera cam;
    // Scene scene(shape, cam);
    // 对物体进行尺度和平移变换
    // scene.shape.scale(DR::Vertor3f(0.6, 0.9, 0.6));
    // scene.shape.translate(DR::Vertor3f(-0.25, -0.25, -2));
    // scene.rendering();
    
    // cout << a1[a2] << endl;

    // DR::Vector3fC aas(1.2, 2.2, 1.2);
    // aas[1] = 3;
    // cout << aas << endl;
    // DR::Vector3fC bbs(1.2, 3.2, 1.2);
    // bbs[0] = 3;
    // cout << bbs << endl;
    // using Index = Packet<int, 8>;
    // for (auto [index, mask]: range<Index>(0, 1000)) {

    // DR::VectorHfC a = zero<DR::VectorHfC>();
    // cout << a << " " << endl;
    
    // cout << abs(-3.4) << endl;
    
    // }
    // test();
    // Shape<Array<float, 10>, Array<float, 20>> ab(5,6);
    // ab.vec[3] = 5;
    return 0;
}
