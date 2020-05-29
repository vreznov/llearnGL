#include "kshader.h"

KShader::KShader()
{

}

KShader::KShader(const char *vertexPath, const char *fragmentPath)
{
    //1. 从文件路径读取顶点/片段着色器
    string vertexCode;
    string fragmentCode;
    ifstream ifs_vShaderFile;
    ifstream ifs_fShaderFile;

    // 保证ifstream对象可以抛出异常
    ifs_vShaderFile.exceptions(ifstream::failbit | ifstream::badbit);
    ifs_fShaderFile.exceptions(ifstream::failbit | ifstream::badbit);

    try {
        ifs_vShaderFile.open(vertexPath, ios::in);
        ifs_fShaderFile.open(fragmentPath);
        stringstream vSS, fSS;
        // 读取文件的缓冲内容到数据流中
        vSS << ifs_vShaderFile.rdbuf();
        fSS << ifs_fShaderFile.rdbuf();
        ifs_vShaderFile.close();
        ifs_fShaderFile.close();
        //转换数据流到string
        vertexCode = vSS.str();
        fragmentCode = fSS.str();
    } catch (ifstream::failure e) {
        cerr << "error load shader file" << endl;
    }

    const char * pvShaderCode = vertexCode.c_str();
    const char * pfShaderCode = fragmentCode.c_str();

    // 编译着色器
    uint vertex, fragment;
    int success;
    char infoLog[512];

    // 顶点着色器
    vertex = glad_glCreateShader(GL_VERTEX_SHADER);
    glad_glShaderSource(vertex, 1, &pvShaderCode, nullptr);
    glad_glCompileShader(vertex);
    // 打印编译错误
    glad_glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
    if(!success){
        glad_glGetShaderInfoLog(vertex, 512, nullptr, infoLog);
    }

    // 片段着色器
    fragment = glad_glCreateShader(GL_FRAGMENT_SHADER);
    glad_glShaderSource(fragment, 1, &pfShaderCode, nullptr);
    glad_glCompileShader(fragment);
    // 打印编译错误
    glad_glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
    if(!success){
        glad_glGetShaderInfoLog(fragment, 512, nullptr, infoLog);
    }

    // 着色器程序
    ID = glad_glCreateProgram();
    glad_glAttachShader(ID, vertex);
    glad_glAttachShader(ID, fragment);
    glad_glLinkProgram(ID);
    // 打印连接错误（如果有的话）
    glad_glGetProgramiv(ID, GL_LINK_STATUS, &success);
    if(!success)
    {
        glGetProgramInfoLog(ID, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }

    // 删除着色器
    glad_glDeleteShader(vertex);
    glad_glDeleteShader(fragment);

}

void KShader::use()
{
    glBindTexture(GL_TEXTURE_2D, texture);
    glad_glUseProgram(ID);
}

void KShader::setBool(const string &name, bool value) const
{
    glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
}

void KShader::setInt(const string &name, int value) const
{
    glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
}

void KShader::setFloat(const string &name, float value) const
{
    glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
}

void KShader::loadTextures()
{
    glad_glGenTextures(1, &texture);
    glad_glBindTexture(GL_TEXTURE_2D, texture);
    // 为当前绑定的纹理对象设置环绕、过滤方式
    glad_glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glad_glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    int width, height, nrChannels;
    uchar *data = stbi_load("texture1.jpg", &width, &height, &nrChannels, 0);

    glad_glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    glad_glGenerateMipmap(GL_TEXTURE_2D);

    // 释放图像
    stbi_image_free(data);

    // 应用纹理

}


