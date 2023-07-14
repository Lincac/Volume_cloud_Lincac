#pragma once

#ifndef PERLIN_H
#define PERLIN_H

#include<iostream>
#include"Shader.h"
#include"Texture.h"
#include"Help.h"

class Perlin {
public:
	Perlin(int w,int h);
	~Perlin();
	unsigned int image,FBO;
	Shader shader;
};

#endif // !PERLIN_H
