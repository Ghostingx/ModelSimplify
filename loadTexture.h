#pragma once
#ifndef LOADTEXTURE_H
#define LOADTEXTURE_H

#include <GL/glut.h>
#include <iostream>
#include<GL/gl.h>
#include<GL/glu.h>
#include<iostream>

#define BMP_Header_Length 54

using namespace std;

//纹理结构体定义
struct TextureImage
{
public:
    GLubyte *imageData;//图像数据
    GLuint bpp;//像素深度
    GLuint width;//图像宽度
    GLuint height;//图像高度
    GLuint texID;//对应的纹理ID
    GLenum format;
    GLint internalFormat;

    ~TextureImage(){
        delete []imageData;
    }
};

//加载TGA图像，生成纹理
GLuint LoadTGATexture(const char *fileName);
//加载PNG图像
TextureImage *ReadPNGFromFile(const char *fileName);
GLuint loadPNGTexture(const char *fileName);
void GetPNGtextureInfo(int color_type, TextureImage *texinfo);
//加载bmp纹理
GLuint loadBMPTexture(const char* fileName);
GLuint loadTexture(const char* fileName);

#endif


