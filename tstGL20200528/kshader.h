#ifndef KSHADER_H
#define KSHADER_H

#include "glad/glad.h"

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <QtGlobal>
#include "stb_image.h"

using namespace std;

class KShader
{
public:
    KShader();

    //程序ID
    unsigned int ID;

    //构造器读取并构建着色器
    KShader(const char* vertexPath, const char* fragmentPath);

    //使用/激活程序
    void use();

    //uniform工具函数
    void setBool(const string &name, bool value) const;
    void setInt(const string &name, int value) const;
    void setFloat(const string &name, float value) const;

    // 载入纹理
    void loadTextures();

    uint texture;
};

#endif // KSHADER_H