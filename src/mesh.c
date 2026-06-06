#ifndef MESH_H
#define MESH_H

#include <glad/glad.h>
#include <glfw/glfw3.h>
#include <cglm/cglm.h>
#include <stdbool.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

#include "texture.c"

typedef struct {
	vec3 Position;
	vec3 Normal;
	vec2 TexCoords;
	vec4 Tangent;
	vec3 Bitangent;
} Vertex;

typedef struct {
	int vertexCount;
	Vertex* vertices;

	int indexCount;
	unsigned int* indices;

	int textureCount;
	Texture* textures;

	unsigned int VAO, VBO, EBO, instanceVBO;
} Mesh;

Mesh createMesh(Vertex* vertices, int vertexCount, unsigned int* indices, int indexCount, Texture* textures, int textureCount);
void setupMesh(Mesh* mesh);
void setupMeshInstanced(Mesh* mesh, mat4* modelMatrices, int instanceCount);
void drawMesh(Mesh* mesh, unsigned int shaderProgram);
void drawMeshInstanced(Mesh* mesh, unsigned int shaderProgram, int instanceCount);
void calculateMeshTangents(Vertex* vertices, int vertexCount, unsigned int* indices, int indexCount);

#endif
#if __INCLUDE_LEVEL__ == 0

Mesh createMesh(Vertex* vertices, int vertexCount, unsigned int* indices, int indexCount, Texture* textures, int textureCount) {
	Mesh mesh = {0};

	mesh.vertexCount = vertexCount;
	mesh.indexCount = indexCount;
	mesh.textureCount = textureCount;

	if (vertices && vertexCount > 0) {
		mesh.vertices = malloc(vertexCount * sizeof(Vertex));
		memcpy(mesh.vertices, vertices, vertexCount * sizeof(Vertex));
	} else {
		mesh.vertices = NULL;
	}

	if (indices && indexCount > 0) {
		mesh.indices = malloc(indexCount * sizeof(unsigned int));
		memcpy(mesh.indices, indices, indexCount * sizeof(unsigned int));
	} else {
		mesh.indices = NULL;
	}

	if (textures && textureCount > 0) {
		mesh.textures = malloc(textureCount * sizeof(Texture));
		memcpy(mesh.textures, textures, textureCount * sizeof(Texture));
	} else {
		mesh.textures = NULL;
	}

	setupMesh(&mesh);
	return mesh;
}

void setupMesh(Mesh* mesh) {
	if (mesh->vertices && mesh->indices) {
		calculateMeshTangents(mesh->vertices, mesh->vertexCount, mesh->indices, mesh->indexCount);
	}

	glGenVertexArrays(1, &mesh->VAO);
	glGenBuffers(1, &mesh->VBO);
	glGenBuffers(1, &mesh->EBO);

	glBindVertexArray(mesh->VAO);
	glBindBuffer(GL_ARRAY_BUFFER, mesh->VBO);
	glBufferData(GL_ARRAY_BUFFER, mesh->vertexCount * sizeof(Vertex), mesh->vertices, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh->indexCount * sizeof(unsigned int), mesh->indices, GL_STATIC_DRAW);

	// positions
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);

	// texture Coordinates
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));

	// normals
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));

	// tangents
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Tangent));


	// bitangents
	glEnableVertexAttribArray(4);
	glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Bitangent));

	glBindVertexArray(0);
}

void setupMeshInstanced(Mesh* mesh, mat4* modelMatrices, int instanceCount) {
	setupMesh(mesh);
	glBindVertexArray(mesh->VAO);

	glGenBuffers(1, &mesh->instanceVBO);
	glBindBuffer(GL_ARRAY_BUFFER, mesh->instanceVBO);
	glBufferData(GL_ARRAY_BUFFER, instanceCount * sizeof(mat4), modelMatrices, GL_STATIC_DRAW);

	int startLocation = 5;
	for (int i = 0; i < 4; i++) {
		glEnableVertexAttribArray(startLocation + i);
		glVertexAttribPointer(
			startLocation + i, 
			4, GL_FLOAT, GL_FALSE, 
			sizeof(mat4), 
			(void*)(i * sizeof(vec4))
		);

		glVertexAttribDivisor(startLocation + i, 1); 
	}

	glBindVertexArray(0);
}

void drawMesh(Mesh* mesh, unsigned int shaderProgram) {
	unsigned int diffuseNr  = 1;
	unsigned int specularNr = 1;
	unsigned int normalNr   = 1;

	glUseProgram(shaderProgram);

	for (int i = 0; i < mesh->textureCount; i++) {
		glActiveTexture(GL_TEXTURE0 + i);

		char uniformName[64];

		if (mesh->textures[i].type == TEXTURE_DIFFUSE) {
			snprintf(uniformName, sizeof(uniformName), "material.diffuse%u", diffuseNr++);
		}
		else if (mesh->textures[i].type == TEXTURE_SPECULAR) {
			snprintf(uniformName, sizeof(uniformName), "material.specular%u", specularNr++);
		}
		else if (mesh->textures[i].type == TEXTURE_NORMAL) {
			snprintf(uniformName, sizeof(uniformName), "material.normal%u", normalNr++);
		}

		glUniform1i(glGetUniformLocation(shaderProgram, uniformName), i);
		glBindTexture(GL_TEXTURE_2D, mesh->textures[i].ID);
	}

	glBindVertexArray(mesh->VAO);
	glDrawElements(GL_TRIANGLES, mesh->indexCount, GL_UNSIGNED_INT, 0);
	
	glBindVertexArray(0);
	glActiveTexture(GL_TEXTURE0);
}

void drawMeshInstanced(Mesh* mesh, unsigned int shaderProgram, int instanceCount) {
	unsigned int diffuseNr  = 1;
	unsigned int specularNr = 1;
	unsigned int normalNr   = 1;

	glUseProgram(shaderProgram);

	for (int i = 0; i < mesh->textureCount; i++) {
		glActiveTexture(GL_TEXTURE0 + i);
		char uniformName[64];

		if (mesh->textures[i].type == TEXTURE_DIFFUSE) {
			snprintf(uniformName, sizeof(uniformName), "material.diffuse%u", diffuseNr++);
		}
		else if (mesh->textures[i].type == TEXTURE_SPECULAR) {
			snprintf(uniformName, sizeof(uniformName), "material.specular%u", specularNr++);
		}
		else if (mesh->textures[i].type == TEXTURE_NORMAL) {
			snprintf(uniformName, sizeof(uniformName), "material.normal%u", normalNr++);
		}

		glUniform1i(glGetUniformLocation(shaderProgram, uniformName), i);
		glBindTexture(GL_TEXTURE_2D, mesh->textures[i].ID);
	}

	glBindVertexArray(mesh->VAO);
	
	glDrawElementsInstanced(GL_TRIANGLES, mesh->indexCount, GL_UNSIGNED_INT, 0, instanceCount);
	
	glBindVertexArray(0);
	glActiveTexture(GL_TEXTURE0);
}

// Implemented after a famous algorithm by Eric Lengyel
// For reference: Computing Tangent Space Basis Vectors for an Arbitrary Mesh
void calculateMeshTangents(Vertex* vertices, int vertexCount, unsigned int* indices, int indexCount) {
	vec3 *tan1 = (vec3*)calloc(vertexCount, sizeof(vec3));
	vec3 *tan2 = (vec3*)calloc(vertexCount, sizeof(vec3));

	if (!tan1 || !tan2) {
		free(tan1);
		free(tan2);
		return;
	}

	for (int i = 0; i < indexCount; i += 3) {
		unsigned int i0 = indices[i];
		unsigned int i1 = indices[i+1];
		unsigned int i2 = indices[i+2];

		Vertex* v0 = &vertices[i0];
		Vertex* v1 = &vertices[i1];
		Vertex* v2 = &vertices[i2];

		float x1 = v1->Position[0] - v0->Position[0];
		float x2 = v2->Position[0] - v0->Position[0];
		float y1 = v1->Position[1] - v0->Position[1];
		float y2 = v2->Position[1] - v0->Position[1];
		float z1 = v1->Position[2] - v0->Position[2];
		float z2 = v2->Position[2] - v0->Position[2];

		float s1 = v1->TexCoords[0] - v0->TexCoords[0];
		float s2 = v2->TexCoords[0] - v0->TexCoords[0];
		float t1 = v1->TexCoords[1] - v0->TexCoords[1];
		float t2 = v2->TexCoords[1] - v0->TexCoords[1];

		float denominator = (s1 * t2 - s2 * t1);
		float r = (fabsf(denominator) < 0.000001f) ? 1.0f : 1.0f / denominator;

		vec3 sdir = {
			(t2 * x1 - t1 * x2) * r,
			(t2 * y1 - t1 * y2) * r,
			(t2 * z1 - t1 * z2) * r
		};

		vec3 tdir = {
			(s1 * x2 - s2 * x1) * r,
			(s1 * y2 - s2 * y1) * r,
			(s1 * z2 - s2 * z1) * r
		};

		glm_vec3_add(tan1[i0], sdir, tan1[i0]);
		glm_vec3_add(tan1[i1], sdir, tan1[i1]);
		glm_vec3_add(tan1[i2], sdir, tan1[i2]);

		glm_vec3_add(tan2[i0], tdir, tan2[i0]);
		glm_vec3_add(tan2[i1], tdir, tan2[i1]);
		glm_vec3_add(tan2[i2], tdir, tan2[i2]);
	}

	for (int i = 0; i < vertexCount; i++) {
		vec3 n, t, projection;
		glm_vec3_copy(vertices[i].Normal, n);
		glm_vec3_copy(tan1[i], t);

		float dot_nt = glm_vec3_dot(n, t);
		glm_vec3_scale(n, dot_nt, projection);
		glm_vec3_sub(t, projection, t);
		glm_vec3_normalize(t);

		vertices[i].Tangent[0] = t[0];
		vertices[i].Tangent[1] = t[1];
		vertices[i].Tangent[2] = t[2];

		vec3 cross_nt;
		glm_vec3_cross(n, t, cross_nt);
		vertices[i].Tangent[3] = (glm_vec3_dot(cross_nt, tan2[i]) < 0.0f) ? -1.0f : 1.0f;

		glm_vec3_scale(cross_nt, vertices[i].Tangent[3], vertices[i].Bitangent);
		glm_vec3_normalize(vertices[i].Bitangent);
	}

	free(tan1);
	free(tan2);
}

#endif
