#pragma once
#include "stb_image.h"
#include <GL/glew.h>
#include <imgui.h>
#include <iostream>
#include "../Images/logo.h"

inline ImTextureID LoadTextureFromMemory_GL(const unsigned char* data, size_t size, int* outWidth, int* outHeight)
{
    if (!data || size == 0)
    {
        std::cout << "[ImageLoader] ERROR: Image data invalid\n";
        return nullptr;
    }

    int width = 0;
    int height = 0;
    int channels = 0;

    unsigned char* pixels = stbi_load_from_memory(
        data,
        (int)size,
        &width,
        &height,
        &channels,
        4 // FORCE RGBA
    );

    if (!pixels)
    {
        std::cout << "[ImageLoader] ERROR: stbi_load_from_memory FAILED\n";
        std::cout << "[ImageLoader] REASON: " << stbi_failure_reason() << "\n";
        return nullptr;
    }

    std::cout << "[ImageLoader] Image decoded: "
        << width << "x" << height
        << " Channels: " << channels << "\n";

    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    // Setup filtering parameters for display
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    // Upload pixels into texture
#if defined(GL_UNPACK_ROW_LENGTH) && !defined(__EMSCRIPTEN__)
    glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
#endif
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);

    stbi_image_free(pixels);

    if (outWidth)  *outWidth = width;
    if (outHeight) *outHeight = height;

    std::cout << "[ImageLoader] SUCCESS: Texture created (ID: " << texture << ")\n";

    return (ImTextureID)(intptr_t)texture;
}
