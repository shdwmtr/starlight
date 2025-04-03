#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <texture.h>
#include <memory.h>
#include <vector>
#include <iostream>
#include <imgui.h>

GLuint logoTexture;
GLuint infoIconTexture;
GLuint closeButtonTexture;
GLuint discordIconTexture;
GLuint gtihubIconTexture;
GLuint backBtnTexture;
GLuint excludedIconTexture;
GLuint errorIconTexture;
GLuint successIconTexture;

bool LoadTextureFromMemory(const void* data, size_t data_size, GLuint* out_texture, int* out_width, int* out_height)
{
    int image_width = 0;
    int image_height = 0;
    unsigned char* image_data = stbi_load_from_memory((const unsigned char*)data, (int)data_size, &image_width, &image_height, NULL, 4);
    if (image_data == NULL)
        return false;

    GLuint image_texture;
    glGenTextures(1, &image_texture);
    glBindTexture(GL_TEXTURE_2D, image_texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image_width, image_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image_data);
    stbi_image_free(image_data);

    *out_texture = image_texture;

    if (out_width)
        *out_width = image_width;

    if (out_height)
        *out_height = image_height;

    return true;
}

void LoadTextures()
{
    LoadTextureFromMemory(logo, sizeof(logo), &logoTexture);
    LoadTextureFromMemory(infoIcon, sizeof(infoIcon), &infoIconTexture);
    LoadTextureFromMemory(closeBtn, sizeof(closeBtn), &closeButtonTexture);
    LoadTextureFromMemory(discordIcon, sizeof(discordIcon), &discordIconTexture);
    LoadTextureFromMemory(githubIcon, sizeof(githubIcon), &gtihubIconTexture);
    LoadTextureFromMemory(backBtn, sizeof(backBtn), &backBtnTexture);
    LoadTextureFromMemory(excludedIcon, sizeof(excludedIcon), &excludedIconTexture);
    LoadTextureFromMemory(errorIcon, sizeof(errorIcon), &errorIconTexture);
    LoadTextureFromMemory(successIcon, sizeof(successIcon), &successIconTexture);
}
