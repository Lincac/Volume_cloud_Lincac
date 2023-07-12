#pragma once

#ifndef TEXTURE_H
#define TEXTURE_H

#include<glad/glad.h>
#include<stb_image.h>
#include<iostream>
#include<vector>

#define INT_CEIL(n,d) (int)ceil((float)n/d)

unsigned int LoadTexture(const char* path);
unsigned int loadCubemap(std::vector<std::string> faces);
void generateTexture2D(int w, int h, unsigned int& tex);
void generateTexture3D(int w, int h, int d, unsigned int& tex);

#endif // !TEXTURE_H
