#include"Cloud.h"
#include"PerlinNoise.h"
#include"WorleyNoise.h"

#include<imgui/imgui.h>
#include<imgui/imgui_impl_glfw.h>
#include<imgui/imgui_impl_opengl3.h>

unsigned int width = 1600;
unsigned int height = 900;

Camera camera(glm::vec3(0.0f, 2000.0f, 0.0f));

float lastX = (float)width / 2.0f;
float lastY = (float)height / 2.0f;
bool firstMouse = true;
bool swimMode = false;

float deltaTime = 0.0f;
float lastFrame = 0.0f;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void mouse_button_callback(GLFWwindow* window, int key, int action, int mods);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);
int main() {
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(width, height, "Volume_Cloud_Lincac", nullptr, nullptr);
	if (window == NULL)
	{
		std::cerr << "error to create window";
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cerr << "load glad error";
		glfwTerminate();
		return -1;
	}

	glEnable(GL_DEPTH_TEST);

	ImGui::CreateContext();
	ImGui::StyleColorsDark();
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 450");

	Cloud cloud(width, height);

	Shader sceneShader("shader/main.vs", "shader/main.fs");
	sceneShader.use();
	sceneShader.setInt("image", 0);

#pragma region pingpong
	Shader shaderBlur("shader/pingpong.vs", "shader/pingpong.fs");
	shaderBlur.use();
	shaderBlur.setInt("image", 0);
	GLuint pingpongFBO[2];
	GLuint pingpongBuffer[2];
	glGenFramebuffers(2, pingpongFBO);
	glGenTextures(2, pingpongBuffer);
	for (GLuint i = 0; i < 2; i++)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[i]);
		glBindTexture(GL_TEXTURE_2D, pingpongBuffer[i]);
		glTexImage2D(
			GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, NULL
		);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glFramebufferTexture2D(
			GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, pingpongBuffer[i], 0
		);
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glBindTexture(GL_TEXTURE_2D, 0);
#pragma endregion

	while (!glfwWindowShouldClose(window))
	{
		float currentFrame = float(glfwGetTime());
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		processInput(window);

		cloud.render(camera);

		GLboolean horizontal = true, first_iteration = true;
		GLuint amount = 2;
		shaderBlur.use();
		for (GLuint i = 0; i < amount; i++)
		{
			glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[horizontal]);
			shaderBlur.setInt("horizontal", horizontal);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, first_iteration ? cloud.raymarchMap : pingpongBuffer[!horizontal]);
			renderQuad();
			horizontal = !horizontal;
			if (first_iteration)
				first_iteration = false;
		}
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glClearColor(1.0, 1.0, 1.0, 1.0);

		sceneShader.use();
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, pingpongBuffer[0]);
		renderQuad();

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		ImGui::Begin("Operate Object", NULL, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_MenuBar);
		{
			ImGui::SliderFloat("coverage", &cloud.coverage, 0.0f, 1.0f,"%.4f");
			ImGui::SliderFloat("cloudSpeed", &cloud.cloudSpeed, 0.0f, 1000.0f);
			ImGui::DragFloat("crispiness", &cloud.crispiness, 0.001f,0.0f, 100.0f);
			ImGui::DragFloat("curliness", &cloud.curliness, 0.0001f, 0.0f, 1.0f, "%.4f");
			ImGui::DragFloat("absorption", &cloud.absorption, 0.001f, 0.0f, 1.0f);
			ImGui::DragFloat("densityFactor", &cloud.densityFactor, 0.0001f, 0.0f, 1.0f, "%.4f");
			ImGui::DragFloat("cloudType", &cloud.cloudType, 0.001f,0.0f, 1.0f);

			ImGui::ColorEdit3("lightColor", (float*)&cloud.lightColor);
			ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

		}
		ImGui::End();

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	glfwTerminate();
	return 0;
}

void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS && swimMode)
		camera.ProcessKeyboard(FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS && swimMode)
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS && swimMode)
		camera.ProcessKeyboard(LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS && swimMode)
		camera.ProcessKeyboard(RIGHT, deltaTime);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = float(xpos);
		lastY = float(ypos);
		firstMouse = false;
	}
	float xoffset = float(xpos) - lastX;
	float yoffset = lastY - float(ypos); // reversed since y-coordinates go from bottom to top

	lastX = float(xpos);
	lastY = float(ypos);

	if (swimMode) {
		camera.ProcessMouseMovement(xoffset, yoffset);
	}
}

void mouse_button_callback(GLFWwindow* window, int key, int action, int mods) {
	if (key == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS) {
		swimMode = !swimMode;
		glfwSetInputMode(window, GLFW_CURSOR, swimMode ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);
	}
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(float(yoffset));
}
