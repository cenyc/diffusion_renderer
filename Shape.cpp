#include "Shape.h"

template<typename Ver, typename Tri>
void Shape<Ver, Tri>::updateStepSize() {
    this->step = norm(this->getVer(this->diagonalVID[0]) - this->getVer(this->diagonalVID[1])) / this->sampleNum;
}

template<typename Ver, typename Tri>
DR::Point Shape<Ver, Tri>::getIntensity1(DR::Point voxelVer) {
    return (*this->id1)[voxelVer[0]][voxelVer[1]][voxelVer[2]];
}

template<typename Ver, typename Tri>
float Shape<Ver, Tri>::getIntensity0(DR::Point voxelVer) {
    return (*this->id0)[voxelVer[0]][voxelVer[1]][voxelVer[2]];
}

template<typename Ver, typename Tri>
DR::Point Shape<Ver, Tri>::getOrgVer() {
    return getVer(this->orgVerID);
}

template<typename Ver, typename Tri>
void Shape<Ver, Tri>::initMinmax () {
    for (size_t i = 0; i < 3; i++)
    {
        this->minmax[0][i] = hmin(this->ver[i]);
        this->minmax[1][i] = hmax(this->ver[i]);
        this->minmax[2][i] = abs(this->minmax[1][i] - this->minmax[0][i]);
    }
}

template<typename Ver, typename Tri>
DR::Point Shape<Ver, Tri>::world2voxel(DR::Point* ptrVer) {
    auto localVer = world2local(ptrVer);
    return local2voxel(&localVer);
}

template<typename Ver, typename Tri>
DR::Point Shape<Ver, Tri>::local2voxel(DR::Point* ptrVer) {
    return ((*ptrVer)/minmax[2]) * DR::Point(DR::ID_WIDTH-1, DR::ID_HEIGHT-1, DR::ID_WIDTH-1);
}

template<typename Ver, typename Tri>
DR::Point Shape<Ver, Tri>::world2local(DR::Point* ptrVer) {
    return (*ptrVer) - getOrgVer();
}

template<typename Ver, typename Tri>
void Shape<Ver, Tri>::translate(DR::Vertor3f tran) {
    this->ver += tran;
    this->initMinmax();
}

template<typename Ver, typename Tri>
void Shape<Ver, Tri>::scale(DR::Vertor3f scale_) {
    DR::Matrix3f scaleTransform(scale_[0], 0.0f, 0.0f,
                                0.0f, scale_[1], 0.0f,
                                0.0f, 0.0f, scale_[2]);
    this->ver = scaleTransform*this->ver;
    this->initMinmax();
    this->updateStepSize();
}

template<typename Ver, typename Tri>
DR::ID3 Shape<Ver, Tri>::getTriIDs(size_t faceID) {
    return slice(this->tri, faceID);
}

template<typename Ver, typename Tri>
DR::Point Shape<Ver, Tri>::getVer(size_t index_){
    return slice(this->ver, index_);
};

template<typename Ver, typename Tri>
HitTriangle Shape<Ver, Tri>::intersect(Ray ray, size_t faceID) {
    float dirT = 0.0, u = 0.0, v = 0.0;
    HitTriangle hitTri = HitTriangle();

    // 根据faceID的索引获取三个顶点的位置
    auto triIDs = getTriIDs(faceID);
    auto v0 = getVer(triIDs.x()),
    v1 = getVer(triIDs.y()),
    v2 = getVer(triIDs.z());

    // e1, e2
    DR::Edge e1 = v1 - v0, 
    e2 = v2 - v0;
    // p
    auto p = cross(ray.dir, e2);
    // determinant
    auto det = dot(e1, p);
    DR::Point t = zero<DR::Point>();

    if (det > 0)
    {
        t = ray.org - v0;
    }else {
        t = v0 - ray.org;
        det = -det;
    }

    if (det < 0.0001) {
        hitTri.isHit = false;
    }else {
        auto u = dot(t, p);
        if (u < 0 || u > det)
        {
            hitTri.isHit = false;
        }else {
            auto q = cross(t, e1);
            auto v = dot(ray.dir, q);
            if (v < 0 || (u+v) > det)   
            {
                hitTri.isHit = false;
            }else {
                float fInvDet = 1.0/det;
                hitTri.dirT = dot(e2, q) * fInvDet;
                hitTri.u = u * fInvDet;
                hitTri.v = v * fInvDet;
            }
        }
    }
    
    return hitTri;
}

template<typename Ver, typename Tri>
void Shape<Ver, Tri>::setParam(float _sigmaA, float _sigmaS, float _hg_G, DR::Density * _desity){
     this->sigmaA = _sigmaA;
     this->sigmaS = _sigmaS;
     this->sigmaT = _sigmaA + _sigmaS;
     this->hg_G   = _hg_G;
     this->density = _desity;
}

template<typename Ver, typename Tri>
float Shape<Ver, Tri>::phaseFunction(float cos_theta){
    float hg = (1 / (4 * pi)) * ((1 - pow(this->hg_G, 2)) / (1 + pow(this->hg_G, 2) - 2 * this->hg_G * cos_theta));
    return hg;
}

template<typename Ver, typename Tri>
float Shape<Ver, Tri>::getMu(int _n){
    float mu = 0.0;
    float cos_t = 0.0;
    // Riemann积分计算一阶矩
    for (int i = 0; i <= _n; i++){
        cos_t = -1.0 + i * (2.0/_n);
        mu += cos_t * this->phaseFunction(cos_t);
    }
    mu *= (3.0 /_n);
    return mu;
}

template<typename Ver, typename Tri>
float Shape<Ver, Tri>::getSigma_Tr(){
    float muBar = this->getMu(100);
    return this->sigmaS * (1 - muBar / 3.0) + this->sigmaA;
}

template<typename Ver, typename Tri>
void Shape<Ver, Tri>::copyValue(DR::Density * _from, DR::Density * _to){
    const int nworker = omp_get_num_procs();
    #pragma omp parallel for num_threads(nworker) schedule(dynamic, 1)
    for(int i = 0;i < DR::ID_WIDTH; i++){
        (*_to)[i] = (*_from)[i];
    }
}

template<typename Ver, typename Tri>
float Shape<Ver, Tri>::countError(DR::Density * _lastValue, DR::Density * _nowValue){
    float error    = 0.0;
    float maxError = 0.0;

    const int nworker = omp_get_num_procs();
    #pragma omp parallel for num_threads(nworker) schedule(dynamic, 1)
    for (int i = 0; i < DR::ID_WIDTH; i++)
    {
        error = hmax(hmax(abs((*_lastValue)[i] - (*_nowValue)[i])));
        if (error > maxError){
            maxError = error;
        }
    }
    return maxError;
}

template<typename Ver, typename Tri>
void Shape<Ver, Tri>::updataBoundary(DR::Density * _id0, DR::Density * _Qri_1, DR::Density * _kappa, float _step){
    float sigama_Tr = this->getSigma_Tr();

    (*_id0)[0] = (2 * (*_kappa)[0] * (*_id0)[1] - 2 * _step * (this->sigmaS / sigama_Tr) * (*_Qri_1)[0])/
                (_step + 2 * (*_kappa)[0]);
    (*_id0)[DR::ID_WIDTH - 1] = (2 * (*_kappa)[DR::ID_WIDTH - 1] * (*_id0)[DR::ID_WIDTH - 2] + 2 * _step * (this->sigmaS / sigama_Tr) * (*_Qri_1)[DR::ID_WIDTH - 1])/
                (_step + 2 * (*_kappa)[DR::ID_WIDTH - 1]);

    const int nworker = omp_get_num_procs();
    #pragma omp parallel for num_threads(nworker) schedule(dynamic, 1)
    for(int i = 0; i < DR::ID_WIDTH; i++){
        (*_id0)[i][0] = (2 * (*_kappa)[i][0] * (*_id0)[i][1]) / (_step + 2 * (*_kappa)[i][0]);
        (*_id0)[i][DR::ID_HEIGHT - 1] = (2 * (*_kappa)[i][DR::ID_HEIGHT - 1] * (*_id0)[i][DR::ID_HEIGHT - 2]) / (_step + 2 * (*_kappa)[i][DR::ID_HEIGHT - 1]);
    }
    
    #pragma omp parallel for num_threads(nworker) schedule(dynamic, 1)
    for(int i = 0; i < DR::ID_WIDTH; i++){
        for (int j = 0; j < DR::ID_HEIGHT; j++)
        {
            (*_id0)[i][j][0] = (2 * (*_kappa)[i][j][0] * (*_id0)[i][j][1]) / (_step + 2 * (*_kappa)[i][j][0]);
            (*_id0)[i][j][DR::ID_WIDTH - 1] = (2 * (*_kappa)[i][j][DR::ID_WIDTH - 1] * (*_id0)[i][j][DR::ID_WIDTH - 2]) / (_step + 2 * (*_kappa)[i][j][DR::ID_WIDTH - 1]);
        }
    }

}

template<typename Ver, typename Tri>
void Shape<Ver, Tri>::updataId0(DR::Density * _id0, DR::Density * _alpha, DR::Density * _kappa, DR::Density * _S, float _step){
    DR::Density *id0_copy = (DR::Density *) malloc(sizeof(zero<DR::Density>()));
    float sigma1 = 0;
    float sigma2 = 0;

    this->copyValue(_id0, id0_copy);
    
    const int nworker = omp_get_num_procs();
    //#pragma omp parallel for num_threads(nworker) schedule(dynamic, 1)
    for(int i = 1;i < DR::ID_WIDTH - 1; i++){
        for(int j = 1; j < DR::ID_HEIGHT - 1; j++){
            for(int k = 1; k < DR::ID_WIDTH - 1; k++){
                sigma1  = ((*_kappa)[i+1][j][k] - (*_kappa)[i-1][j][k])*
                          ((*id0_copy)[i+1][j][k] - (*id0_copy)[i-1][j][k])+
                          ((*_kappa)[i][j+1][k] - (*_kappa)[i][j-1][k])*
                          ((*id0_copy)[i][j+1][k] - (*id0_copy)[i][j-1][k])+
                          ((*_kappa)[i][j][k+1] - (*_kappa)[i][j][k-1])*
                          ((*id0_copy)[i][j][k+1] - (*id0_copy)[i][j][k-1]);
                
                sigma2  = (*id0_copy)[i+1][j][k] + (*id0_copy)[i-1][j][k] +
                          (*id0_copy)[i][j+1][k] + (*id0_copy)[i][j-1][k] +
                          (*id0_copy)[i][j][k+1] + (*id0_copy)[i][j][k-1];
                
                (*_id0)[i][j][k] = (_step * _step * (*_S)[i][j][k] + 0.25 * sigma1 + (*_kappa)[i][j][k] * sigma2)/
                                   (_step * _step * (*_alpha)[i][j][k] + 6 * (*_kappa)[i][j][k]);
                
            }
        }
    }
}

template<typename Ver, typename Tri>
DR::ID1 * Shape<Ver, Tri>::getGrid(DR::Density * _id0, DR::ID1 * idGrid, float step){
    const int nworker = omp_get_num_procs();
    #pragma omp parallel for num_threads(nworker) schedule(dynamic, 1)
    for(int i = 0;i < DR::ID_WIDTH; i++){
        (*idGrid)[i] = 0;
    }

    #pragma omp parallel for num_threads(nworker) schedule(dynamic, 1)
    for (int i = 1; i < DR::ID_WIDTH - 1; i++){
        for (int j = 1; j < DR::ID_HEIGHT - 1; j++){
            for (int k = 1; k < DR::ID_WIDTH - 1; k++){
                (*idGrid)[i][j][k][0] = ((*_id0)[i+1][j][k] - (*_id0)[i-1][j][k])/(2 * step);
                (*idGrid)[i][j][k][1] = ((*_id0)[i][j+1][k] - (*_id0)[i][j-1][k])/(2 * step);
                (*idGrid)[i][j][k][2] = ((*_id0)[i][j][k+1] - (*_id0)[i][j][k-1])/(2 * step);
            }
        }
    }
    
    return idGrid;
}

template<typename Ver, typename Tri>
DR::Density * Shape<Ver, Tri>::getId0(float light_intensity, float step){
    // 初始化id0， 均为1
    setDensityValue(this->id0, 1);
    
    // 初始化Ir i,光强经过消光单方向递减
    DR::Density *Ir_i = (DR::Density *) malloc(sizeof(zero<DR::Density>()));
    setDensityValue(Ir_i, 1);

    const int nworker = omp_get_num_procs();
    #pragma omp parallel for num_threads(nworker) schedule(dynamic, 1)
    for(int i = 0;i < DR::ID_WIDTH; i++){
        (*Ir_i)[i] *= light_intensity * exp((-1) * this->sigmaT * step * (i + 1) * 0.5 );
    }

    // Qri_0和Qri_1
    #pragma omp parallel for num_threads(nworker) schedule(dynamic, 1)
    for(int i = 0; i < DR::ID_WIDTH; i++){
        (*this->Qri_0)[i] = (*Ir_i)[i] / (4 * pi);
    }

    float mu = this->getMu(1000);
    #pragma omp parallel for num_threads(nworker) schedule(dynamic, 1)
    for(int i = 0; i < DR::ID_WIDTH; i++){
        (*this->Qri_1)[i] = (*Ir_i)[i] * mu / (4 * pi);
    }

    // 求Qri_1的梯度
    DR::Density *Qri_1_grad = (DR::Density *) malloc(sizeof(zero<DR::Density>()));
    *Qri_1_grad = zero<DR::Density>();
    #pragma omp parallel for num_threads(nworker) schedule(dynamic, 1)
    for(int i = 0; i < DR::ID_WIDTH - 1; i++){
        (*Qri_1_grad)[i] = (*Qri_1)[i + 1] - (*Qri_1)[i];
    }

    float sigmaTr = this->getSigma_Tr();
    // 求解S
    *this->S = zero<DR::Density>();
    #pragma omp parallel for num_threads(nworker) schedule(dynamic, 1)
    for(int i = 0; i < DR::ID_WIDTH; i++){
        (*this->S)[i] = (this->sigmaS * (*this->density)[i] * (*this->Qri_0)[i]) -
        (this->sigmaS / sigmaTr)*(*Qri_1_grad)[i];
    }

    // 求解kappa
    this->kappa = (DR::Density *) malloc(sizeof(zero<DR::Density>()));
    #pragma omp parallel for num_threads(nworker) schedule(dynamic, 1)
    for(int i = 0; i < DR::ID_WIDTH; i++){
        (*this->kappa)[i] = (1.0)/(sigmaTr * ((*this->density)[i]));
    }

    // 求解alpha
    #pragma omp parallel for num_threads(nworker) schedule(dynamic, 1)
    for(int i = 0; i < DR::ID_WIDTH; i++){
        (*this->alpha)[i] = this->sigmaA * (*this->density)[i];
    }

    this->updataBoundary(this->id0, this->Qri_1, this->kappa, step);
    DR::Density *id0_last = (DR::Density *) malloc(sizeof(zero<DR::Density>()));
    
    // 松弛法求解
    float error = 0.0;
    int64_t start_time = CurrentTimeMillis();
    
    for(int i = 0;i < 1000; i++){
        this->copyValue(this->id0, id0_last);
        this->updataId0(this->id0, this->alpha, this->kappa, this->S, step);
        this->updataBoundary(this->id0, this->Qri_1, this->kappa, step);
        error = this->countError(id0_last, this->id0);
        if(i % 10 == 0){
            cout<<"iterat:"<<i<<" error:"<<error<<endl;
        }
        if(error < 0.01){
            cout<<"id0 has been converged"<<endl;
            break;
        }
    }
    std::cout << "time is: " <<CurrentTimeMillis() - start_time << " ms" << std::endl;
    
    return this->id0;
}

template<typename Ver, typename Tri>
DR::ID1 * Shape<Ver, Tri>::getId1(DR::Density * _id0, float step){
    // 初始化id0， 均为1
    this->getGrid(_id0, this->id1, step);
    for(int i = 0;i < DR::ID_WIDTH; i++){
        for(int j = 0; j < DR::ID_HEIGHT; j++){
            for(int k = 0; k < DR::ID_WIDTH; k++){
                (*this->id1)[i][j][k][0] = (-1) * (*this->kappa)[i][j][k] * (*this->id1)[i][j][k][0] + this->sigmaS * (*this->density)[i][j][k] * (*this->Qri_1)[i][j][k];
                (*this->id1)[i][j][k][1] = (-1) * (*this->kappa)[i][j][k] * (*this->id1)[i][j][k][1];
                (*this->id1)[i][j][k][2] = (-1) * (*this->kappa)[i][j][k] * (*this->id1)[i][j][k][2];
            }
        }
    }
    return this->id1;
}

void setDensityValue(DR::Density * den, float _value){
    const int nworker = omp_get_num_procs();
    #pragma omp parallel for num_threads(nworker) schedule(dynamic, 1)
    for(int i = 0; i < DR::ID_WIDTH; i++){
        (*den)[i] = _value;
    }
}
