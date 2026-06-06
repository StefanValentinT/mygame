#ifndef TEXTURE_H
#define TEXTURE_H

#include <glad/glad.h>
#include <stb/stb_image.h>

typedef enum {
	TEXTURE_DIFFUSE,
	TEXTURE_SPECULAR,
	TEXTURE_NORMAL
} TextureType;

typedef struct {
	unsigned int ID;
	TextureType type;
} Texture;

Texture loadTexture(char* path, TextureType type);

#endif
#if __INCLUDE_LEVEL__ == 0

Texture loadTexture(char* path, TextureType type) {
	int width, height, nrChannels;
	unsigned char *data = stbi_load(path, &width, &height, &nrChannels, 0);
	if (!data) {
		fprintf(stderr, "Texture Error: File not found at path: %s\n", path);
		exit(EXIT_FAILURE);
	}

	GLenum format = GL_RGB;
	if (nrChannels == 1) {
		format = GL_RED;
	} else if (nrChannels == 3) {
		format = GL_RGB;
	} else if (nrChannels == 4) {
		format = GL_RGBA;
	}

	unsigned int texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
	glGenerateMipmap(GL_TEXTURE_2D);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	stbi_image_free(data);
	return (Texture){ .ID = texture, .type = type };
}

#endif
