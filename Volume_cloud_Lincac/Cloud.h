#pragma once

#ifndef CLOUD_H
#define CLOUD_H

#include<glad/glad.h>
#include<GLFW/glfw3.h>

#include"Camera.h"
#include"Texture.h"
#include"ComputeShader.h"
#include"Shader.h"
#include<iostream>

class Cloud {
public:
	Cloud(int w,int h);
	~Cloud();

	void render(Camera& camera);

	void initshader();
	void inittexture();

	unsigned int width, height;

	ComputeShader perlinworley, worley, weather,raymarch;
	unsigned int perlinworleyMap, worleyMap, weatherMap, raymarchMap;

	float coverage;
	float cloudSpeed;
	float crispiness;
	float curliness;
	float absorption;
	float densityFactor;
	float cloudType;

	glm::vec3 lightColor;

};

#endif // !CLOUD_H
