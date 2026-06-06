#ifndef SHADER_H
#define SHADER_H

#include <glad/glad.h>
#include <glfw/glfw3.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <cglm/cglm.h>

typedef struct {
	unsigned int ID;
} Shader;

Shader makeShader(const char* vertexPath, const char* fragmentPath);
void delete(const Shader* shader);
void use(const Shader* shader);
void setBool(const Shader* shader, const char* name, bool value);
void setInt(const Shader* shader, const char* name, int value);
void setFloat(const Shader* shader, const char* name, float value);
void setVec2(const Shader* shader, const char* name, const vec2 value);
void setVec3(const Shader* shader, const char* name, const vec3 value);
void setVec4(const Shader* shader, const char* name, const vec4 value);
void setVec2f(const Shader* shader, const char* name, float x, float y);
void setVec3f(const Shader* shader, const char* name, float x, float y, float z);
void setVec4f(const Shader* shader, const char* name, float x, float y, float z, float w);
void setMat2(const Shader* shader, const char* name, const mat2 mat);
void setMat3(const Shader* shader, const char* name, const mat3 mat);
void setMat4(const Shader* shader, const char* name, const mat4 mat);

#endif
#if __INCLUDE_LEVEL__ == 0

static char* readFile(const char* path) {
	FILE* file = fopen(path, "rb");
	if (!file) {
		printf("Failed to open file: %s\n", path);
		return NULL;
	}

	fseek(file, 0, SEEK_END);
	long length = ftell(file);
	fseek(file, 0, SEEK_SET);

	char* buffer = malloc(length + 1);
	if (buffer) {
		fread(buffer, 1, length, file);
		buffer[length] = '\0';
	}
	fclose(file);
	return buffer;
}

static void checkShader(unsigned int shader) {
	int  success;
	char infoLog[512];
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

	if(!success)
	{
		glGetShaderInfoLog(shader, 512, NULL, infoLog);
		printf("Error. Shader Compilation failed: %s", infoLog);
		exit(EXIT_FAILURE);
	}
 
}

Shader makeShader(const char* vertexPath, const char* fragmentPath) {
	Shader shader = {0};

	char* vertexSource = readFile(vertexPath);
	char* fragmentSource = readFile(fragmentPath);

	if (!vertexSource || !fragmentSource) {
		free(vertexSource);
		free(fragmentSource);
		return shader;
	}

	const char* vCode = vertexSource;
	const char* fCode = fragmentSource;

	unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vCode, NULL);
	glCompileShader(vertexShader);
	checkShader(vertexShader);

	unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fCode, NULL);
	glCompileShader(fragmentShader);
	checkShader(fragmentShader);

	shader.ID = glCreateProgram();
	glAttachShader(shader.ID, vertexShader);
	glAttachShader(shader.ID, fragmentShader);
	glLinkProgram(shader.ID);

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
	free(vertexSource);
	free(fragmentSource);

	return shader;
}

void delete(const Shader* shader){
	glDeleteProgram(shader->ID);
}

void use(const Shader* shader) {
	glUseProgram(shader->ID);
}

void setBool(const Shader* shader, const char* name, bool value) {
	glUniform1i(glGetUniformLocation(shader->ID, name), (int)value);
}

void setInt(const Shader* shader, const char* name, int value) {
	glUniform1i(glGetUniformLocation(shader->ID, name), value); 
}

void setFloat(const Shader* shader, const char* name, float value) {
	glUniform1f(glGetUniformLocation(shader->ID, name), value); 
}

void setVec2(const Shader* shader, const char* name, const vec2 value) {
	glUniform2fv(glGetUniformLocation(shader->ID, name), 1, (float*)value);
}

void setVec3(const Shader* shader, const char* name, const vec3 value) {
	glUniform3fv(glGetUniformLocation(shader->ID, name), 1, (float*)value);
}

void setVec4(const Shader* shader, const char* name, const vec4 value) {
	glUniform4fv(glGetUniformLocation(shader->ID, name), 1, (float*)value);
}

void setVec2f(const Shader* shader, const char* name, float x, float y) {
	glUniform2f(glGetUniformLocation(shader->ID, name), x, y);
}

void setVec3f(const Shader* shader, const char* name, float x, float y, float z) {
	glUniform3f(glGetUniformLocation(shader->ID, name), x, y, z);
}

void setVec4f(const Shader* shader, const char* name, float x, float y, float z, float w) {
	glUniform4f(glGetUniformLocation(shader->ID, name), x, y, z, w);
}

void setMat2(const Shader* shader, const char* name, const mat2 mat) {
	glUniformMatrix2fv(glGetUniformLocation(shader->ID, name), 1, GL_FALSE, (float*)mat);
}

void setMat3(const Shader* shader, const char* name, const mat3 mat) {
	glUniformMatrix3fv(glGetUniformLocation(shader->ID, name), 1, GL_FALSE, (float*)mat);
}

void setMat4(const Shader* shader, const char* name, const mat4 mat) {
	glUniformMatrix4fv(glGetUniformLocation(shader->ID, name), 1, GL_FALSE, (float*)mat);
}

#endif
