#pragma once

#ifndef WORLEY_H
#define WORLEY_H

#include<iostream>
#include"Shader.h"
#include"Texture.h"
#include"Help.h"

class Worley {
public:
	Worley(int w,int h);
	~Worley();

	unsigned int image, FBO;
	Shader shader;
};

#endif // !WORLEY_H
