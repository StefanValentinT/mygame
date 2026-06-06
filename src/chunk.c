#ifndef CHUNK_H
#define CHUNK_H

#include "block.c"
#include "mesh.c"
#include <cglm/cglm.h>

typedef struct {
	BlockType blocks[4096];
	Mesh mesh;
	int blockCount;
} Chunk;

Chunk createChunk(void);
void initChunkTerrain(Chunk* chunk);
void generateChunkMesh(Chunk* chunk, Mesh* baseMesh);
void drawChunk(Chunk* chunk, unsigned int shaderID);

#endif
#if __INCLUDE_LEVEL__ == 0

static const int CHUNK_WIDTH  = 16;
static const int CHUNK_HEIGHT = 16;
static const int CHUNK_DEPTH  = 16;
static const int CHUNK_SIZE   = 4096;

int getBlockIndex(int x, int y, int z) {
	return x + (y * CHUNK_WIDTH) + (z * CHUNK_WIDTH * CHUNK_HEIGHT);
}

Chunk createChunk(void) {
	Chunk chunk;
	chunk.blockCount = 0;
	initChunkTerrain(&chunk);
	return chunk;
}

void initChunkTerrain(Chunk* chunk) {
	for (int y = 0; y < CHUNK_HEIGHT; y++) {
		for (int x = 0; x < CHUNK_WIDTH; x++) {
			for (int z = 0; z < CHUNK_DEPTH; z++) {
				int index = getBlockIndex(x, y, z);
				
				if (y < 10) {
					chunk->blocks[index] = BLOCK_GRASS;
				} else {
					chunk->blocks[index] = BLOCK_AIR;
				}
			}
		}
	}
}

void generateChunkMesh(Chunk* chunk, Mesh* baseMesh) {
	mat4* modelMatrices = malloc(CHUNK_SIZE * sizeof(mat4));
	int activeBlockCount = 0;

	for (int y = 0; y < CHUNK_HEIGHT; y++) {
		for (int x = 0; x < CHUNK_WIDTH; x++) {
			for (int z = 0; z < CHUNK_DEPTH; z++) {
				int index = getBlockIndex(x, y, z);
				BlockType type = chunk->blocks[index];

				if (BlockRegistry[type].properties.isSolid) {
					glm_mat4_identity(modelMatrices[activeBlockCount]);
					glm_translate(modelMatrices[activeBlockCount], (vec3){(float)x, (float)y, (float)z});
					activeBlockCount++;
				}
			}
		}
	}

	chunk->mesh = *baseMesh;
	setupMeshInstanced(&chunk->mesh, modelMatrices, activeBlockCount);
	chunk->blockCount = activeBlockCount;
	free(modelMatrices);
}

void drawChunk(Chunk* chunk, unsigned int shaderID) {
	if (chunk->blockCount > 0) {
		drawMeshInstanced(&chunk->mesh, shaderID, chunk->blockCount);
	}
}

#endif
