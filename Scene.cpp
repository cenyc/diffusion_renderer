#include "Scene.h"
#include "include/svpng.inc"

void Scene::rendering() {
    Utils::msg("Start scene rendering.");
    int count = 1;
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
            for (size_t k = 0; k < this->shape.faceNum; k++)
            {
                HitTriangle hitTran = this->shape.intersect(ray, k);
                if (hitTran.isHit)
                {
                    
                }
                
            }
        }
    }
    
    // cout << max(2.3, 5) << endl;
    // cout << dot(bb, cc[0]) << " " << dot(bb, cc[1]) << " " << dot(bb, cc[2]) << endl;
    // cout << dot(cc, bb) << endl;
    // auto ray = this->cam.getRay(0, 0);
    // this->shape.intersect(ray, 1);
    // this->shape.getVer(1);
}

void Scene::saveGrayImg(string filename) {
    unsigned char rgb[DR::WIDTH * DR::HEIGHT * 3], *p = rgb;
    unsigned x, y;
    int colorValue = 0;
    
    FILE *fp = fopen(filename.c_str(), "wb");
    for (y = 0; y < DR::WIDTH; y++)
        for (x = 0; x < DR::HEIGHT; x++) {
            colorValue = 0;
            if ((this->imgBuff[0][y][x] >= 0) && (this->imgBuff[0][y][x] < 256)) // 判断色值范围
            {
                colorValue = (int)this->imgBuff[0][y][x] + 100;
            }
            
            *p++ = (unsigned char)colorValue;    // R
            *p++ = (unsigned char)colorValue;    // G
            *p++ = (unsigned char)colorValue;    // B
        }
    svpng(fp, DR::WIDTH, DR::HEIGHT, rgb, 0);
    fclose(fp);
    Utils::msg("Finished Save Gray Img.");
}
