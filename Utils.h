#ifndef __UTILITIES_H__
#define __UTILITIES_H__
#include "common.h"
#include "Shape.h"
/**
 * @brief 工具结构体，乱七八糟的功能都放这里。
 * 
 */

struct Utils {
    /**
     * @brief 读取off文件
     * 
     * @param path off文件路径
     */
    static Shape<DR::F3X, DR::S3X> readOFF(string path);
    /**
     * @brief 打印string
     * 
     * @param msg string字符信息
     * @param stop true则打印完成后结束程序运行；false则打印完成后继续执行
     */
    static void msg(string msg, bool stop=false);

    /**
     * @brief 读取npy文件数据
     * 
     * @tparam value 
     * @param path 文件路径
     * @return value 返回的读取数据，若是id0为64*64*96大小的数组，若是id1为64*64*96*3大小的数组
     */
    static bool readId0(string path, float Id0[DR::ID_WIDTH][DR::ID_HEIGHT][DR::ID_WIDTH]);
    static bool readId1(string path, DR::Point Id1[DR::ID_WIDTH][DR::ID_HEIGHT][DR::ID_WIDTH]);

    /**
     * @brief 动态创建二维数组
     * 
     * @tparam dtype 数组类型
     * @param n 一维大小
     * @param m 二维大小
     * @return dtype** 
     */
    template<typename dtype> static dtype** newArray2D(int n, int m);

};
#endif