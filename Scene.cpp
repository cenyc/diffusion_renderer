#include "Scene.h"
void Scene::rendering() {
    Utils::msg("Start rendering secne.");
    int count = 1;
    for (size_t i = 0; i < (*imgBuff).size(); i++)
    {
        auto imgBuffColumn = (*imgBuff)[i];
        for (size_t j = 0; j < imgBuffColumn.size(); j++)
        {
            // auto ray = this->cam.getRay(i, j);
            // this->shape.intersect();
        }
    }
    auto ray = this->cam.getRay(0, 0);
    this->shape.intersect(ray);
    this->shape.getVer(1);
}
