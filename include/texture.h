#pragma once
#include <GL/gl.h>

extern GLuint logoTexture;
extern GLuint infoIconTexture;
extern GLuint closeButtonTexture;
extern GLuint discordIconTexture;
extern GLuint gtihubIconTexture;
extern GLuint backBtnTexture;
extern GLuint excludedIconTexture;

bool LoadTextureFromMemory(const void* data, size_t data_size, GLuint* out_texture, int* out_width = nullptr, int* out_height = nullptr);
void LoadTextures();