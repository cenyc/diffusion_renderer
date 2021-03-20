#include "Scene.h"
#include "Shape.cpp"
#include "include/svpng.inc"


float Scene::attenuation(float dist) {
    return exp(-1.8 * dist);
}

/**
 * @brief 在参与介质内部求Iri
 * 
 * @param ray
 *              ray.org - 射线起点；
 *              ray.dir - 射线方向
 * @return float 
 */
float Scene::getIri(Ray ray) {
    // float Iri = 0.0;
    // int faceNum = this->shape->faceNum;
    // bool isHit = false;
    // DR::Point hitPoint;
    // for (int i = 0; i < faceNum; i++)
    // {
    //     HitTriangle hitT = this->shape->intersect(ray, i);
    //     if (!isHit&&hitT.isHit)
    //     {
    //         // cout << "hit!!!!!!!!1" << endl;
    //         isHit = true;
    //         hitPoint = ray.org + hitT.dirT * ray.dir;
    //         break;
    //     }
    // }
    // if (isHit)
    // {
    //     DR::Point ray2HitPoint = hitPoint - ray.org;
    //     Iri += getIri(normalize(ray2HitPoint), norm(ray2HitPoint));
    //     // cout << "Iri is " << Iri << endl;
    // }


    return getIri(DR::Point(-1, 0, 0), ray.org.x() - this->shape->aabb.min.x());
}

/**
 * @brief 射线的起点在参与介质外部时，获取由环境光带来的辐射
 * 
 * @param dir 射线的方向单位向量
 * @param dist 穿过参与介质的厚度
 * @return float 
 */
float Scene::getIri(DR::Point dir, float dist) {
    float Iri = 0.0;
    if (dot(DR::ENV_LIGTH_DIR, dir) == -1)
    {
        Iri = DR::ENV_INTSITY * attenuation(dist);
    }
    return Iri;
}

void Scene::computeLoss() {
    auto diff = (*targetImgBuff - *imgBuff);
    cout << diff << endl;
    for (size_t i = 0; i < DR::WIDTH; i++)
    {
        for (size_t j = 0; j < DR::HEIGHT; j++)
        {
            for (size_t k = 0; k < 3; k++)
            {
                this->loss += enoki::pow(diff[k][i][j], 2);
            }
        }
    }
}

void Scene::rendering(DR::ImgBuff3* ptrImgBuff) {
    Utils::msg("Start scene rendering.");
    int count = 1;
    float max_t = 0.0;
    for (size_t y = 0; y < DR::HEIGHT; y++)
    {
        // auto imgBuffColumn = (*imgBuff)[i];
        for (size_t x = 0; x < DR::WIDTH; x++)
        {
            float u = (x+drand48())/DR::WIDTH,
            v = (y+drand48())/DR::HEIGHT;
            auto ray = this->cam.getRay(u, v);

            HitRecord hitRecord = this->shape->aabb.hit(ray);
            if (hitRecord.isHit)
            {
                auto intensity = rayMarching(hitRecord);

                (*ptrImgBuff)[0][x][y] = intensity*this->lightColor[0];
                (*ptrImgBuff)[1][x][y] = intensity*this->lightColor[1];
                (*ptrImgBuff)[2][x][y] = intensity*this->lightColor[2];
            }
            
        }
    }
}

void Scene::renderingParallel(DR::ImgBuff3* ptrImgBuff) {
    Utils::msg("Start scene rendering.");
    int count = 1;
    float max_t = 0.0;
    for (size_t y = 0; y < DR::HEIGHT; y++)
    {
        // auto imgBuffColumn = (*imgBuff)[i];
        for (size_t x = 0; x < DR::WIDTH; x++)
        {
            float u = (x+drand48())/DR::WIDTH,
            v = (y+drand48())/DR::HEIGHT;
            auto ray = this->cam.getRay(u, v);
            bool isHit = false;
            int hitNum = 0;

            HitRecord hitRecord = this->shape->aabb.hit(ray);
            if (hitRecord.isHit)
            {
                auto intensity = rayMarching(hitRecord);
                
                float dirT = hitRecord.length;
                (*ptrImgBuff)[0][x][y] = intensity*0.8;
                (*ptrImgBuff)[1][x][y] = intensity*0.7;
                (*ptrImgBuff)[2][x][y] = intensity*0.2;
            }
        }
    }
}

/**
 * @brief HENYEY-GREENSTEIN PHASE FUNCTION
 * 
 * 这里用的是一种近似HG算法，公式为 p = (1-k^2)/(4*M_PI*(1+k*cos(theta))^2)。
 * 详见 《Real-time Scattering》P30
 * 为了和Stam扩散论文的公式系数一致，这里p*4*M_PI。
 * 
 * @param inDir 入射方向的单位向量
 * @param outDir 出射方向的单位向量
 * @return float 
 */
float hg(DR::Point inDir, DR::Point outDir) {
    float k = 0.3;
    float cosine_theta = dot(inDir, outDir);
    return (1-pow(k, 2))/pow((1+k*cosine_theta), 2);
}

float Scene::rayMarching(const HitRecord& hitRecord) {
    // float intensity = 0.0;
    float pathLength = hitRecord.length;
    auto currentPoint = hitRecord.p0;
    float currentLength = 0.0;
    // 视线前进方向
    auto rayMarchingDir = hitRecord.dir;
    // 平行光方向
    DR::Point lightDir = DR::Point(1, 0, 0);
    // 计算Iri
    float Iri = getIri(rayMarchingDir, pathLength);
    float Id = 0.0;
    while (currentLength <= pathLength)
    {
        auto voxelPoint = round(this->shape->world2voxel(&currentPoint));
        auto id0 = this->shape->getIntensity0(voxelPoint);
        auto id1 = this->shape->getIntensity1(voxelPoint);
        // 从采样点射向光源方向
        Ray ray2Light(currentPoint, -DR::ENV_LIGTH_DIR);
        float jms = id0*DR::ALBEDO + dot(id1, -rayMarchingDir/**采样点辐射光强射入视点的方向*/)*DR::ALBEDO/3.0; //此处差一个u拔系数没有乘
        float jss = DR::ALBEDO/(4*M_PI)*hg(DR::ENV_LIGTH_DIR, -rayMarchingDir)*getIri(ray2Light);
        // // float jss = getIri(ray2Light);
        Id += (jss+jms) * attenuation(currentLength) * 1.8;
        
        currentPoint += rayMarchingDir*this->shape->aabb.step;
        currentLength += this->shape->aabb.step;
        // Id += jss;
        // Id += jss;
    }

    return Iri + Id;
}


void Scene::saveColorImg(string filename, DR::ImgBuff3 *ptrImgBuff) {
    unsigned char rgb[DR::WIDTH * DR::HEIGHT * 3], *p = rgb;
    int x, y;
    int colorValue[3]   = {0, 0, 0};
    float nomalValue[3] = {0.0, 0.0, 0.0};
    float maxValue   = __FLT_MIN__;
    DR::Float tmpVal;
    
    FILE *fp = fopen(filename.c_str(), "wb");
    for (x = 0; x < DR::WIDTH; x++){
        for(y = 0; y < DR::HEIGHT; y++){
            for (size_t i = 0; i < 3; i++)
            {
                tmpVal = DR::Float((*ptrImgBuff)[i][x][y]);
                if(tmpVal[0] > maxValue){
                    maxValue = tmpVal[0];
                }
            }
        }
    }
    for (y = DR::HEIGHT - 1; y >= 0; y--){
        for (x = 0; x < DR::WIDTH; x++) {
            // foreach color channel
            for (size_t i = 0; i < 3; i++)
            {
                // tmpVal = DR::Float((*this->imgBuff)[i][x][y]);
                nomalValue[i] = DR::Float((*ptrImgBuff)[i][x][y])[0] / maxValue * 255.0;
                colorValue[i] = int(nomalValue[i]);
                // cout << "tmpVal is " << tmpVal << endl;
                *p++ = (unsigned char)colorValue[i];
            }
        }
    }
    svpng(fp, DR::WIDTH, DR::HEIGHT, rgb, 0);
    fclose(fp);
    Utils::msg("Finished Save Gray Img.");
}

void Scene::saveGrayImg(string filename) {
    unsigned char rgb[DR::WIDTH * DR::HEIGHT * 3], *p = rgb;
    int x, y;
    int colorValue   = 0;
    float nomalValue = 0.0;
    float maxValue   = 0.0;
    
    // FILE *fp = fopen(filename.c_str(), "wb");
    // for (x = 0; x < DR::WIDTH; x++){
    //     for(y = 0; y < DR::HEIGHT; y++){
    //         if((*this->imgBuff)[x][y] > maxValue){
    //             maxValue = (*this->imgBuff)[x][y];
    //         }
    //     }
    // }
    
    // for (y = DR::HEIGHT - 1; y >= 0; y--){
    //     for (x = 0; x < DR::WIDTH; x++) {
    //         nomalValue = (*this->imgBuff)[x][y] / maxValue * 255.0;
    //         colorValue = int(nomalValue);
            
    //         *p++ = (unsigned char)colorValue;    // R
    //         *p++ = (unsigned char)colorValue;    // G
    //         *p++ = (unsigned char)colorValue;    // B
    //     }
    // }
    // svpng(fp, DR::WIDTH, DR::HEIGHT, rgb, 0);
    // fclose(fp);
    // Utils::msg("Finished Save Gray Img.");
}
