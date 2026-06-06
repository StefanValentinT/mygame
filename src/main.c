#Imports

#include <stdio.h>
#include <stdlib.h> 
#include <glad/glad.h>
#include <glfw/glfw3.h>
#include <math.h>
#include <stb/stb_image.h>
#include <cglm/cglm.h>

#include "shader.h"
#include "camera.h"
#include "texture.h"
#include "mesh.h"

#Impl
const unsigned int SCR_WIDTH = 600;
const unsigned int SCR_HEIGHT = 600;

//     H--------G
//    /|       /|
//   / |      / |
//  A--------B  |
//  |  E-----|--F
//  | /      | /
//  |/       |/
//  D--------C
Vertex cubeVertices[] = {
	// front
	{{-0.5f, -0.5f,  0.5f}, { 0.0f,  0.0f,  1.0f}, {0.0f, 0.0f}, {0}, {0}}, // D
	{{ 0.5f, -0.5f,  0.5f}, { 0.0f,  0.0f,  1.0f}, {1.0f, 0.0f}, {0}, {0}}, // C
	{{ 0.5f,  0.5f,  0.5f}, { 0.0f,  0.0f,  1.0f}, {1.0f, 1.0f}, {0}, {0}}, // B
	{{-0.5f,  0.5f,  0.5f}, { 0.0f,  0.0f,  1.0f}, {0.0f, 1.0f}, {0}, {0}}, // A

	// back
	{{-0.5f, -0.5f, -0.5f}, { 0.0f,  0.0f, -1.0f}, {0.0f, 0.0f}, {0}, {0}}, // E
	{{-0.5f,  0.5f, -0.5f}, { 0.0f,  0.0f, -1.0f}, {0.0f, 1.0f}, {0}, {0}}, // H
	{{ 0.5f,  0.5f, -0.5f}, { 0.0f,  0.0f, -1.0f}, {1.0f, 1.0f}, {0}, {0}}, // G
	{{ 0.5f, -0.5f, -0.5f}, { 0.0f,  0.0f, -1.0f}, {1.0f, 0.0f}, {0}, {0}}, // F

	// left
	{{-0.5f,  0.5f,  0.5f}, {-1.0f,  0.0f,  0.0f}, {1.0f, 1.0f}, {0}, {0}}, // A
	{{-0.5f,  0.5f, -0.5f}, {-1.0f,  0.0f,  0.0f}, {0.0f, 1.0f}, {0}, {0}}, // H
	{{-0.5f, -0.5f, -0.5f}, {-1.0f,  0.0f,  0.0f}, {0.0f, 0.0f}, {0}, {0}}, // E
	{{-0.5f, -0.5f,  0.5f}, {-1.0f,  0.0f,  0.0f}, {1.0f, 0.0f}, {0}, {0}}, // D

	// right
	{{ 0.5f,  0.5f,  0.5f}, { 1.0f,  0.0f,  0.0f}, {0.0f, 1.0f}, {0}, {0}}, // B
	{{ 0.5f, -0.5f,  0.5f}, { 1.0f,  0.0f,  0.0f}, {0.0f, 0.0f}, {0}, {0}}, // C
	{{ 0.5f, -0.5f, -0.5f}, { 1.0f,  0.0f,  0.0f}, {1.0f, 0.0f}, {0}, {0}}, // F
	{{ 0.5f,  0.5f, -0.5f}, { 1.0f,  0.0f,  0.0f}, {1.0f, 1.0f}, {0}, {0}}, // G

	// bottom
	{{-0.5f, -0.5f, -0.5f}, { 0.0f, -1.0f,  0.0f}, {0.0f, 1.0f}, {0}, {0}}, // E
	{{ 0.5f, -0.5f, -0.5f}, { 0.0f, -1.0f,  0.0f}, {1.0f, 1.0f}, {0}, {0}}, // F
	{{ 0.5f, -0.5f,  0.5f}, { 0.0f, -1.0f,  0.0f}, {1.0f, 0.0f}, {0}, {0}}, // C
	{{-0.5f, -0.5f,  0.5f}, { 0.0f, -1.0f,  0.0f}, {0.0f, 0.0f}, {0}, {0}}, // D

	// top
	{{-0.5f,  0.5f, -0.5f}, { 0.0f,  1.0f,  0.0f}, {0.0f, 1.0f}, {0}, {0}}, // H
	{{-0.5f,  0.5f,  0.5f}, { 0.0f,  1.0f,  0.0f}, {0.0f, 0.0f}, {0}, {0}}, // A
	{{ 0.5f,  0.5f,  0.5f}, { 0.0f,  1.0f,  0.0f}, {1.0f, 0.0f}, {0}, {0}}, // B
	{{ 0.5f,  0.5f, -0.5f}, { 0.0f,  1.0f,  0.0f}, {1.0f, 1.0f}, {0}, {0}}  // G
};

unsigned int cubeIndices[] = {
	0, 1, 2,     0, 2, 3,     // front
	4, 5, 6,     4, 6, 7,     // back
	8, 9, 10,    8, 10, 11,   // left
	12, 13, 14,  12, 14, 15,  // right
	16, 17, 18,  16, 18, 19,  // bottom
	20, 21, 22,  20, 22, 23   // top
};

float deltaTime = 0.0f;
float lastFrame = 0.0f;

Camera cam;
bool firstMouse = true;
float lastX = 300.0f;
float lastY = 300.0f;
bool locked = false;

int main() {
	GLFWwindow* window = setup();

	cam = cameraDefault();

	Shader objectShader = makeShader("shaders/shader.vert", "shaders/shader.frag");

	Texture diffuseTex = loadTexture("res/blocks/grass/grass_block_top.png", TEXTURE_DIFFUSE);
	Texture normalTex   = loadTexture("res/blocks/grass/grass_block_top_n.png", TEXTURE_NORMAL);
	Texture cubeTextures[] = { diffuseTex, normalTex };

	mat4 model;
	glm_mat4_identity(model);
	mat4 view;
	mat4 projection;

	int numInstances = 100 * 100;
	mat4* cubeMatrices = malloc(numInstances * sizeof(mat4));
	int index = 0;
	for (int x = 0; x < 100; x++) {
		for (int z = 0; z < 100; z++) {
			glm_mat4_identity(cubeMatrices[index]);
			glm_translate(cubeMatrices[index], (vec3){(float)x, 0.0f, (float)z});
			index++;
		}
	}
	Mesh grassCubeMesh = createMesh(cubeVertices, 24, cubeIndices, 36, cubeTextures, 2);
	setupMeshInstanced(&grassCubeMesh, cubeMatrices, numInstances);
	free(cubeMatrices);

	while (!glfwWindowShouldClose(window)) {
		updateDelta(window);
		processInput(window);
		clearScreen();
		
		// render

		use(&objectShader);
		setVec3(&objectShader, "viewPos", cam.pos);
		setVec3f(&objectShader, "light.ambient", 0.35f, 0.35f, 0.35f);
		setVec3f(&objectShader, "light.diffuse", 0.4f, 0.4f, 0.4f);
		setVec3f(&objectShader, "light.specular", 0.1f, 0.1f, 0.1f);
		setVec3f(&objectShader, "light.direction", -0.2f, -1.0f, -0.3f);
		setVec3f(&objectShader, "light.color", 1.0f, 0.85f, 0.4f);
		setFloat(&objectShader, "material.shininess", 2.0f);

		cameraGetViewMatrix(&cam, view);
		setMat4(&objectShader, "view", view);
		glm_perspective(glm_rad(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f, projection);
		setMat4(&objectShader, "projection", projection);
		
		drawMeshInstanced(&grassCubeMesh, objectShader.ID, 10000);

		// end render

		glfwSwapBuffers(window);
		glfwPollEvents();
	}


	glfwTerminate();
	return 0;
}

void clearScreen(){
	glClearColor(0.5f, 0.8f, 0.9f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

GLFWwindow* setup() {
	stbi_set_flip_vertically_on_load(true);

	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	#ifdef __APPLE__
		glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	#endif

	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "My Game", NULL, NULL);
	if (window == NULL) {
		printf("Failed to create GLFW window");
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback); 
	glfwSwapInterval(1);
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		printf("Failed to initialize GLAD");
		exit(EXIT_FAILURE);
	}
	glEnable(GL_DEPTH_TEST);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	return window;
}

void updateDelta(GLFWwindow *window) {
	float currentFrame = (float)glfwGetTime();
	deltaTime = currentFrame - lastFrame;
	lastFrame = currentFrame; 

	if (deltaTime > 0.000001f) {
		float fps = 1.0f / deltaTime;
		char title[50];
		snprintf(title, sizeof(title), "My Game; FPS: %.1f", fps);
		glfwSetWindowTitle(window, title);
	}
}

void processInput(GLFWwindow *window) {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		locked = false;
	}
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		locked = true;
		firstMouse = true;
	}
	
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		cameraProcessKeyboard(&cam, FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		cameraProcessKeyboard(&cam, BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		cameraProcessKeyboard(&cam, LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		cameraProcessKeyboard(&cam, RIGHT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
		cameraProcessKeyboard(&cam, UP, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
		cameraProcessKeyboard(&cam, DOWN, deltaTime);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (locked)
	{
		if (firstMouse)
		{
			lastX = (float)xpos;
			lastY = (float)ypos;
			firstMouse = false;
		}
		float xoffset = (float)xpos - lastX;
		float yoffset = lastY - (float)ypos;
		lastX = (float)xpos;
		lastY = (float)ypos;

		cameraProcessMouseMovementConstrained(&cam, xoffset, yoffset);
	}
}

void framebuffer_size_callback(GLFWwindow *_window, int width, int height) {
	glViewport(0, 0, width, height);
}
