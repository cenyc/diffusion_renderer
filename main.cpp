#include "common.h"
#include "Utils.h"
#include "Shape.h"
#include "Camera.h"
#include "Scene.h"

// #include <cuda_runtime_api.h>
// // #include <cuda_device_runtime_api.h>
// #include <device_launch_parameters.h>

template <typename Value> Value srgb_gamma(Value x) {
    return select(
        x <= 0.0031308f,
        x * 12.92f,
        pow(x * 1.055f, 1.f / 2.4f) - 0.055f
    );
}
int main(int argc,char *argv[]) {
    // using FloatD = DiffArray<DR::FloatC>;
    // FloatD a = 1.0;
    // cout << a << endl;
    return 0;
}
int main1(int argc,char *argv[]) {
    Shape<DR::F3X, DR::S3X> shape;
    // Utils::readOFF("../data/cube.off", shape);
    Camera cam;
    Scene scene(cam, &shape);
    // 对物体进行尺度和平移变换
    scene.shape->scale(DR::Point(0.6, 0.9, 0.6)*2);
    scene.shape->translate(DR::Point(-0.65, -0.75, -2.5));

    // 生成目标图像
    scene.renderingParallel(scene.targetImgBuff);
    scene.saveColorImg("../results/target.png", scene.targetImgBuff);

    // 改变光源颜色
    scene.lightColor = DR::DiffArrayf3(0.1, 0.5, 0.8);
    using Index = Array<Packet<uint32_t, 100>, 3>;
    for (size_t i = 0; i < 1000; i++)
    {
        DR::DF loss = 0.0, sum = 0.0;
        set_requires_gradient(scene.lightColor);
        scene.rendering(scene.imgBuff);
        scene.saveColorImg("../results/img.png", scene.imgBuff);

        // 进行梯度优化
        loss = scene.computeLoss(scene.targetImgBuff, scene.imgBuff);
        backward(loss);
        cout << "loss is " << loss << ", gradient is " << gradient(scene.lightColor) << endl;
        scene.lightColor -= gradient(scene.lightColor)*0.1;
        cout << "light color is " << scene.lightColor << endl;
        // loss = 0.0;
        (*scene.imgBuff) = zero<DR::ImgBuff3>();
    }
    return 0;
}
