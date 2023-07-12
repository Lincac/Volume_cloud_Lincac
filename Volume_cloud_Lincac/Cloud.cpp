#include"Cloud.h"

Cloud::Cloud(int w,int h) : width(w),height(h) {
	perlinworleyMap = 0;
	worleyMap = 0;
	weatherMap = 0;
	raymarchMap = 0;

	coverage = 0.45;
	cloudSpeed = 450.0;
	crispiness = 40;
	curliness = 0.1;
	absorption = 0.35;
	densityFactor = 0.02;
	cloudType = 1.0;

	lightColor = glm::vec3(1.0, 1.0, 1.0) * glm::vec3(1.1, 1.1, 0.95);

	initshader();
	inittexture();
}

Cloud::~Cloud() {
	glDeleteTextures(1, &perlinworleyMap);
	glDeleteTextures(1, &worleyMap);
	glDeleteTextures(1, &weatherMap);
	glDeleteTextures(1, &raymarchMap);

	glDeleteProgram(perlinworley.ID);
	glDeleteProgram(worley.ID);
}

void Cloud::render(Camera& camera) {
	glm::mat4 view = camera.GetViewMatrix();
	glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)width / (float)height, 0.1f, 10000000.0f);

	raymarch.use();
	raymarch.setVec2("Resolution", glm::vec2((float)width ,(float)height));
	raymarch.setFloat("iTime", float(glfwGetTime()));
	raymarch.setMat4("inv_proj", glm::inverse(projection));
	raymarch.setMat4("inv_view", glm::inverse(view));

	raymarch.setVec3("lightColor", lightColor);
	raymarch.setVec3("lightDirection", glm::normalize(glm::vec3(-.5, 0.5, 1.0)));

	raymarch.setFloat("coverage", coverage);
	raymarch.setFloat("cloudSpeed", cloudSpeed);
	raymarch.setFloat("crispiness", crispiness);
	raymarch.setFloat("curliness", curliness);
	raymarch.setFloat("absorption", absorption * 0.01);
	raymarch.setFloat("densityFactor", densityFactor);
	raymarch.setFloat("cloudType", cloudType);

	raymarch.setVec3("cameraPosition", camera.Position);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_3D, perlinworleyMap);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_3D, worleyMap);
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, weatherMap);
	
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, raymarchMap);
	glBindImageTexture(0, raymarchMap, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);
	glDispatchCompute(INT_CEIL(width, 16), INT_CEIL(height, 16), 1);
	glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
}

void Cloud::initshader() {
	perlinworley = ComputeShader("shader/perlinworley.comp");
	worley = ComputeShader("shader/worley.comp");
	weather = ComputeShader("shader/weahter.comp");

	raymarch = ComputeShader("shader/Raymarch.comp");
	raymarch.use();
	raymarch.setInt("perlinworley", 1);
	raymarch.setInt("worley", 2);
	raymarch.setInt("weather", 3);
}


void Cloud::inittexture() {
	if (!perlinworleyMap) {
		generateTexture3D(128, 128, 128, perlinworleyMap);
		perlinworley.use();
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_3D, perlinworleyMap);
		glBindImageTexture(0, perlinworleyMap, 0, GL_TRUE, 0, GL_READ_WRITE, GL_RGBA8);
		glDispatchCompute(INT_CEIL(128, 4), INT_CEIL(128, 4), INT_CEIL(128, 4));
		glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
		glGenerateMipmap(GL_TEXTURE_3D);
	}

	if (!worleyMap) {
		generateTexture3D(32, 32, 32, worleyMap);
		worley.use();
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_3D, worleyMap);
		glBindImageTexture(0, worleyMap, 0, GL_TRUE, 0, GL_READ_WRITE, GL_RGBA8);
		glDispatchCompute(INT_CEIL(32, 4), INT_CEIL(32, 4), INT_CEIL(32, 4));
		glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
		glGenerateMipmap(GL_TEXTURE_3D);
	}

	if (!weatherMap) {
		//weatherMap = LoadTexture("resource/weather3.png");
		generateTexture2D(1024, 1024,weatherMap);
		weather.use();
		weather.setVec3("seed", glm::vec3(0.0f));
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, weatherMap);
		glBindImageTexture(0, weatherMap, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);
		glDispatchCompute(INT_CEIL(1024, 8), INT_CEIL(1024, 8), 1);
		glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
	}

	if (!raymarchMap) {
		generateTexture2D(width, height, raymarchMap);
	}
}
