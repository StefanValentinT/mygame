#ifndef BLOCK_H
#define BLOCK_H

#include "texture.c"

typedef enum {
	BLOCK_AIR = 0,
	BLOCK_GRASS,
	BLOCK_DIRT,
	BLOCK_STONE,
	BLOCK_WOOD,
	BLOCK_TYPE_COUNT
} BlockType;

typedef struct {
	bool isSolid;

	int textureIdTop;
	int textureIdFront;
	int textureIdBack;
	int textureIdLeft;
	int textureIdRight;
	int textureIdBottom;

	int textureIdTopNormal;
	int textureIdFrontNormal;
	int textureIdBackNormal;
	int textureIdLeftNormal;
	int textureIdRightNormal;
	int textureIdBottomNormal;

} BlockProperties;

typedef struct{
	BlockType type;
	BlockProperties properties;
} RegisterEntry;

extern RegisterEntry BlockRegistry[BLOCK_TYPE_COUNT];

int loadBlockTexture(const char* localPath, TextureType type);
void loadAndRegisterAllBlocks(void);
void initBlockRegistry(void);

#endif
#if __INCLUDE_LEVEL__ == 0

RegisterEntry BlockRegistry[BLOCK_TYPE_COUNT];

const char* basePath = "res/blocks/";
const char* diffuseExtension = ".png";
const char* normalExtension = "_n.png";
const char* grassTextureTop = "grass/grass_top";
const char* grassTextureSide = "grass/grass_side";
const char* grassTextureBottom = "grass/dirt";

int loadBlockTexture(const char* localPath, TextureType type) {
	char fullPath[256];
	const char* ext = (type == TEXTURE_NORMAL) ? normalExtension : diffuseExtension;
	snprintf(fullPath, sizeof(fullPath), "%s%s%s", basePath, localPath, ext);
	Texture tex = loadTexture(fullPath, type);
	return tex.ID; 
}

void loadAndRegisterAllBlocks(void) {
	initBlockRegistry();

	int grassTopID  = loadBlockTexture(grassTextureTop, TEXTURE_DIFFUSE);
	int grassSideID = loadBlockTexture(grassTextureSide, TEXTURE_DIFFUSE);
	int dirtID      = loadBlockTexture(grassTextureBottom, TEXTURE_DIFFUSE);

	int grassTopNormalID  = loadBlockTexture(grassTextureTop, TEXTURE_NORMAL);
	int grassSideNormalID = loadBlockTexture(grassTextureSide, TEXTURE_NORMAL);
	int dirtNormalID      = loadBlockTexture(grassTextureBottom, TEXTURE_NORMAL);

	BlockRegistry[BLOCK_GRASS].properties.isSolid = true;
	
	BlockRegistry[BLOCK_GRASS].properties.textureIdTop    = grassTopID;
	BlockRegistry[BLOCK_GRASS].properties.textureIdBottom = dirtID;
	BlockRegistry[BLOCK_GRASS].properties.textureIdFront  = grassSideID;
	BlockRegistry[BLOCK_GRASS].properties.textureIdBack   = grassSideID;
	BlockRegistry[BLOCK_GRASS].properties.textureIdLeft   = grassSideID;
	BlockRegistry[BLOCK_GRASS].properties.textureIdRight  = grassSideID;

	BlockRegistry[BLOCK_GRASS].properties.textureIdTopNormal    = grassTopNormalID;
	BlockRegistry[BLOCK_GRASS].properties.textureIdBottomNormal = dirtNormalID;
	BlockRegistry[BLOCK_GRASS].properties.textureIdFrontNormal  = grassSideNormalID;
	BlockRegistry[BLOCK_GRASS].properties.textureIdBackNormal   = grassSideNormalID;
	BlockRegistry[BLOCK_GRASS].properties.textureIdLeftNormal   = grassSideNormalID;
	BlockRegistry[BLOCK_GRASS].properties.textureIdRightNormal  = grassSideNormalID;
}

void initBlockRegistry(void) {
	BlockRegistry[BLOCK_AIR] = (RegisterEntry){
		.type = BLOCK_AIR,
		.properties = {
			.isSolid = false,
			.textureIdTop = -1, .textureIdBottom = -1,
			.textureIdFront = -1, .textureIdBack = -1,
			.textureIdLeft = -1, .textureIdRight = -1,
			.textureIdTopNormal = -1, .textureIdBottomNormal = -1,
			.textureIdFrontNormal = -1, .textureIdBackNormal = -1,
			.textureIdLeftNormal = -1, .textureIdRightNormal = -1
		}
	};

	for (int i = 1; i < BLOCK_TYPE_COUNT; i++) {
		BlockRegistry[i].type = (BlockType)i;
		BlockRegistry[i].properties.isSolid = true;
		
		BlockRegistry[i].properties.textureIdTop = -1;
		BlockRegistry[i].properties.textureIdBottom = -1;
		BlockRegistry[i].properties.textureIdFront = -1;
		BlockRegistry[i].properties.textureIdBack = -1;
		BlockRegistry[i].properties.textureIdLeft = -1;
		BlockRegistry[i].properties.textureIdRight = -1;

		BlockRegistry[i].properties.textureIdTopNormal = -1;
		BlockRegistry[i].properties.textureIdBottomNormal = -1;
		BlockRegistry[i].properties.textureIdFrontNormal = -1;
		BlockRegistry[i].properties.textureIdBackNormal = -1;
		BlockRegistry[i].properties.textureIdLeftNormal = -1;
		BlockRegistry[i].properties.textureIdRightNormal = -1;
	}
}

#endif
