#include "Scene.h"
#include "include/svpng.inc"




void Scene::rendering() {
    Utils::msg("Start scene rendering.");
    int count = 1;
    float max_t = 0.0;
    // CPU内核个数（32）
    const int nworker = omp_get_num_procs();
    #pragma omp parallel for num_threads(nworker) schedule(dynamic, 1)
    for (size_t y = 0; y < DR::HEIGHT; y++)
    {
        // auto imgBuffColumn = (*imgBuff)[i];
        for (size_t x = 0; x < DR::WIDTH; x++)
        {
            auto ray = this->cam.getRay(x, y);
            float dirTMax = __FLT_MIN__,
            dirTMin = __FLT_MAX__;
            // 遍历所有的面看是否与光线相交
            for (size_t k = 0; k < this->shape->faceNum; k++)
            {
                HitTriangle hitTran = this->shape->intersect(ray, k);
                if (hitTran.isHit)
                {
                    dirTMax = max(dirTMax, hitTran.dirT);
                    dirTMin = min(dirTMin, hitTran.dirT);
                }
                
            }

            // 如果穿过参与介质
            if (dirTMax - dirTMin > 0.0001)
            {
                DR::Point nearPoint = ray.org + ray.dir * dirTMin;
                DR::Point farPoint = ray.org + ray.dir * dirTMax;

                PtrInteraction nearInter = new Interaction(nearPoint, ray.dir),
                farInter = new Interaction(farPoint, ray.dir);
                (*this->imgBuff)[x][y] = rayMarching(nearInter, farInter);
            //     // cout << nearInter << endl;
            }
            
        }
    }
    // cout << this->shape->orgVer << endl;
    // cout << max(2.3, 5) << endl;
    // cout << dot(bb, cc[0]) << " " << dot(bb, cc[1]) << " " << dot(bb, cc[2]) << endl;
    // cout << dot(cc, bb) << endl;
    // auto ray = this->cam.getRay(0, 0);
    // this->shape.intersect(ray, 1);
    // this->shape.getVer(1);
}

float Scene::rayMarching(PtrInteraction nearPoint, PtrInteraction farPoint) {
    float intensity = 0.0;
    float pathLength = norm((farPoint->hitPoint - nearPoint->hitPoint));
    auto currentPoint = nearPoint->hitPoint;
    float currentLength = 0.0;
    while (currentLength <= pathLength)
    {
        auto voxelPoint = round(this->shape->world2voxel(&currentPoint));
        
        auto id0 = this->shape->getIntensity0(voxelPoint);
        auto id1 = this->shape->getIntensity1(voxelPoint);
        intensity += id0 + dot(id1, nearPoint->dir);
        // 更新位置
        currentPoint += farPoint->dir*this->shape->step;
        currentLength += this->shape->step;
    }

    return intensity;
}


void Scene::saveGrayImg(string filename) {
    unsigned char rgb[DR::WIDTH * DR::HEIGHT * 3], *p = rgb;
    int x, y;
    int colorValue = 0;
    
    FILE *fp = fopen(filename.c_str(), "wb");

    for (y = DR::HEIGHT - 1; y >= 0; y--){
        for (x = 0; x < DR::WIDTH; x++) {
            colorValue = 0;
            if (((*this->imgBuff)[x][y] >= 0) && ((*this->imgBuff)[x][y] < 256)) // 判断色值范围
            {
                colorValue = (int)(*this->imgBuff)[x][y];
            }
            
            *p++ = (unsigned char)colorValue;    // R
            *p++ = (unsigned char)colorValue;    // G
            *p++ = (unsigned char)colorValue;    // B
        }
    }

    svpng(fp, DR::WIDTH, DR::HEIGHT, rgb, 0);
    fclose(fp);
    Utils::msg("Finished Save Gray Img.");
}
