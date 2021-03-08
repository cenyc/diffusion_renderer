#include "common.h"
#include "Utils.h"
#include "Shape.h"
#include "Camera.h"
#include "Scene.h"
// #include <cuda_runtime_api.h>
// // #include <cuda_device_runtime_api.h>
// #include <device_launch_parameters.h>
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

    //     auto aa = gather<Float10C>(result.data(), pair.first, pair.second);

    //     cout << "result = " << aa << endl;  
    // }
    /*
    读取Id0和Id1:
    if(Utils::readId0("./data/id0.txt", Id0)){
        cout<<Id0[0][0][0]<<endl;
        cout<<Id0[0][0][1]<<endl;
        cout<<Id0[DR::ID_WIDTH - 1][DR::ID_HEIGHT - 1][DR::ID_WIDTH - 1]<<endl;
    }
    if(Utils::readId1("./data/id1.txt", Id1)){
        cout<<Id1[0][0][0]<<endl;
        cout<<Id1[0][0][1]<<endl;
        cout<<Id1[DR::ID_WIDTH - 1][DR::ID_HEIGHT - 1][DR::ID_WIDTH - 1]<<endl;
    }
    */
   /*
    保存png图片 注* 每像素值需0~255之间
    string fileName = "rgb.png";
    scene.saveGrayImg(fileName);
    */
}

int main(int argc,char *argv[]){    
    // cuda_set_log_level(3);
    Shape<DR::F3X, DR::S3X> shape;
    Utils::readOFF("../data/cube.off", shape);
    Camera cam;
    Scene scene(cam, &shape);
    // 对物体进行尺度和平移变换
    scene.shape->scale(DR::Vertor3f(0.6, 0.9, 0.6)*2);
    scene.shape->translate(DR::Vertor3f(-0.65, -0.75, -4));
 
    scene.rendering();
    scene.saveGrayImg("../data/gray.png");
    return 0;
}
