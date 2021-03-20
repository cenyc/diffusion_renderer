#include "common.h"
#include "Utils.h"
#include "Shape.h"
#include "Camera.h"
#include "Scene.h"

#include "Shape.cpp"

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

int main(int argc,char *argv[]) {
    using A = Array<float, 3>;
    A a(1,2,3);
    cout << a << endl;
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
   /*
    求解id0 和id1
    DR::Density *den = (DR::Density *) malloc(sizeof(zero<DR::Density>()));
    *den = zero<DR::Density>();
    //设置密度
    setDensityValue(den, 0.5);
    //设置参数
    shape.setParam(5.06, 5.0, 0.4, den);
    DR::Density *id0 = NULL;
    id0 = shape.getId0(1000, 0.0625);
    DR::ID1 *id1 = NULL;
    id1 = shape.getId1(id0, 0.0625);
   */
}
int main1(int argc,char *argv[]) {
    Shape<DR::F3X, DR::S3X> shape;
    // Utils::readOFF("../data/cube.off", shape);
    Camera cam;
    Scene scene(cam, &shape);
    DR::DF loss = 0.0;
    // 对物体进行尺度和平移变换
    scene.shape->scale(DR::Point(0.6, 0.9, 0.6)*2);
    scene.shape->translate(DR::Point(-0.65, -0.75, -2.5));

    // 生成目标图像
    scene.renderingParallel(scene.targetImgBuff);
    scene.saveColorImg("../results/target.png", scene.targetImgBuff);

    // 改变光源颜色
    scene.lightColor = DR::DiffArrayf3(0.1, 0.5, 0.8);

    for (size_t i = 0; i < 1000; i++)
    {
        set_requires_gradient(scene.lightColor);
        scene.rendering(scene.imgBuff);
        scene.saveColorImg("../results/img.png", scene.imgBuff);
        DR::DF test = 1.4;
        // 进行梯度优化
        auto diff = (*scene.targetImgBuff) - (*scene.imgBuff);
        for (size_t i = 0; i < DR::WIDTH; i++)
        {
            for (size_t j = 0; j < DR::HEIGHT; j++)
            {
                // diff += enoki::pow((*scene.imgBuff)[0][i][j] - test, 2);
                loss += enoki::pow(diff[0][i][j], 2) + enoki::pow(diff[1][i][j], 2) + enoki::pow(diff[2][i][j], 2);
                // loss += enoki::pow((*scene.imgBuff), 2);
                // cout << "loss is " << loss << endl;
            }
        }
        // loss = loss * 1.0/(DR::WIDTH*DR::HEIGHT);
        backward(loss);
        cout << "loss is " << loss << ", gradient is " << gradient(scene.lightColor) << endl;
        scene.lightColor -= gradient(scene.lightColor)*0.0001;
        cout << "light color is " << scene.lightColor << endl;
        loss = 0.0;
        (*scene.imgBuff) = zero<DR::ImgBuff3>();
        scene.saveColorImg("../results/img1.png", scene.imgBuff);

    }
    

    
    // loss = 0.0;
    return 0;
}

// int main1(int argc,char *argv[]){    
//     // cuda_set_log_level(3);
//     Shape<DR::F3X, DR::S3X> shape;
//     Utils::readOFF("../data/cube.off", shape);
//     Camera cam;
//     Scene scene(cam, &shape);
//     DR::DF loss = 0.0;
//     // 对物体进行尺度和平移变换
//     scene.shape->scale(DR::Vertor3f(0.6, 0.9, 0.6)*2);
//     scene.shape->translate(DR::Vertor3f(-0.65, -0.75, -2.5));

//     // 生成目标图像
//     scene.renderingParallel(scene.targetImgBuff);
//     scene.saveColorImg("../results/target.png", scene.targetImgBuff);
//     // 更改光照颜色
//     scene.lightColor = DR::DiffArrayf3(0.1, 0.5, 0.8);
//     set_requires_gradient(scene.lightColor);
//     scene.rendering(scene.imgBuff);
//     // scene.saveColorImg("../results/img.png", scene.imgBuff);
//     // (*scene.imgBuff) -= (*scene.targetImgBuff);
//     // 进行梯度优化
//     for (size_t i = 0; i < DR::WIDTH; i++)
//     {
//         for (size_t j = 0; j < DR::HEIGHT; j++)
//         {
//             loss += enoki::pow((*scene.imgBuff)[0][i][j], 2) + enoki::pow((*scene.imgBuff)[1][i][j], 2) + enoki::pow((*scene.imgBuff)[2][i][j], 2);
//         }
//     }
//     loss *= 1.0/(DR::WIDTH*DR::HEIGHT);
//     backward(loss);
//     cout << "loss is " << loss << ", gradient is " << gradient(scene.lightColor) << endl;
//     loss = 0.0;
//     return 0;
// }
