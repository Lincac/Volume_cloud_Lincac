#include"PerlinNoise.h"

Perlin::Perlin(int w,int h) {
	generateTexture2D(w, h, image);
	shader = Shader("shader/noise/perlin.vs", "shader/noise/perlin.fs");

	glGenFramebuffers(1, &FBO);
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);
	glBindTexture(GL_TEXTURE_2D, image);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, image, 0);
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glBindTexture(GL_TEXTURE_2D, 0);

	glBindFramebuffer(GL_FRAMEBUFFER, FBO);
	glClear(GL_COLOR_BUFFER_BIT);
	shader.use();
	shader.setVec2("resolution", glm::vec2(w, h));
	shader.setFloat("scale", 16);
	renderQuad();
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

Perlin::~Perlin(){}

