#include "Utils.h"
int msgCount = 1;
void Utils::readOFF(string path, Shape<DR::F3X, DR::S3X>& shape) {
    Utils::msg("Start reading off file.");
    ifstream infile;
    char buffer[256];
    int  verNum = 0, triNum = 0, **tri;
    float a, b, c, d, **vec;
    DR::F3X verBuff;
    DR::S3X triBuff;
    infile.open(path, ios::in);
    if (!infile.is_open())
        Utils::msg("Unable to open file.", true);
    
    // 读取off文件数据
    int count = 0;
    while (!infile.eof())
    {
        infile.getline(buffer, 100);
        // cout << buffer << endl;
        if (count > 0)
        {
            sscanf(buffer, "%f %f %f %f", &a, &b, &c, &d);
            if (count == 1) { // 获取顶点数和面数，并创建顶点和面索引的存储数组。
                verNum = a; triNum = b;
                // vec = Utils::newArray2D<float>(3, verNum);
                // tri = Utils::newArray2D<int>(3, triNum);
                set_slices(verBuff, verNum);
                set_slices(triBuff, triNum);
            }else if (count > 1 && count < verNum + 2) // 获取顶点坐标
            {   
                verBuff[0][count-2] = a;
                verBuff[1][count-2] = b;
                verBuff[2][count-2] = c;
            }else{ // 获取面索引
                triBuff[0][count-2-verNum] = b;
                triBuff[1][count-2-verNum] = c;
                triBuff[2][count-2-verNum] = d;
            }
        }
        ++count;
    }
    infile.close();
    shape.verNum = verNum;
    shape.faceNum = triNum;
    shape.ver = verBuff;
    shape.tri = triBuff;
    shape.initMinmax();
    shape.updateStepSize();
    readId0("../data/id0.txt", *shape.id0);
    readId1("../data/id1.txt", *shape.id1);

    ostringstream ost;
    ost << "Finished reading off file, the verNum is " << verNum << ", triNum is " << triNum << ".";
    Utils::msg(ost.str());

}


void Utils::msg(string msg, bool stop) {
    string line;
    line.assign(msgCount, ' ');
    cout << "*";
    cout << "[line " << msgCount++ << "th] " << msg << endl;
    // cout << "\\" << endl;
    if (stop)
        exit(EXIT_FAILURE);
}

template<typename dtype> dtype** Utils::newArray2D(int n, int m) {
    dtype **arr;
    arr = new dtype*[n];
    for (int i = 0; i < n; i++)
    {
        arr[i] = new dtype[m];
    }
    return arr;
}

bool Utils::readId0(string path, DR::ID0 &id0) {
    Utils::msg("Start reading Id0 file.");
    
    ifstream infile;
    float item;
    infile.open(path, ios::in);
    if (!infile.is_open()){
        Utils::msg("Unable to open file.", true);
        return false;
    }
    
    // 开始读取
    for(int i = 0;i < DR::ID_WIDTH; i++){
        for(int j = 0; j < DR::ID_HEIGHT; j++){
            for(int k = 0; k < DR::ID_WIDTH; k++){
                infile>>item;
                id0[i][j][k] = item;
            }
        }
    }
    infile.close();
    Utils::msg("Finished reading Id0 file.");
    
    return true;
}

bool Utils::readId1(string path, DR::ID1 &id1) {
    Utils::msg("Start reading Id1 file.");
    
    ifstream infile;
    float a,b,c;
    infile.open(path, ios::in);
    if (!infile.is_open()){
        Utils::msg("Unable to open file.", true);
        return false;
    }
    
    // 开始读取
    for(int i = 0;i < DR::ID_WIDTH; i++){
        for(int j = 0; j < DR::ID_HEIGHT; j++){
            for(int k = 0; k < DR::ID_WIDTH; k++){
                infile>>a>>b>>c;
                id1[i][j][k] = DR::Point(a, b, c);;
            }
        }
    }
    infile.close();
    Utils::msg("Finished reading Id1 file.");
    
    return true;
}