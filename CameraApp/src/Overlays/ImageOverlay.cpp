/*
* Copyright 2016-2025
* This file is copyrighted by onexip GmbH.
* All rights reserved.
*
* Date:   2025-05-05
* Author: Thorsten Knoelker
*/

#ifndef __APPLE__
    #ifdef USE_GLES
        #include <gles2.h>
    #else
        #include <gl.h>
    #endif
#endif

#include "ImageOverlay.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <iostream>
#include "WindowUtils.h"


ImageOverlay::ImageOverlay(const std::string& filename) {
    this->filename = filename;
}

void ImageOverlay::setSize(int desiredWidth, int desiredHeight) {
    this->desiredWidth = desiredWidth;
    this->desiredHeight = desiredHeight;
}

void ImageOverlay::init() {
    if (filename.empty()) {
        return;
    }

    texture = loadTextureFromFile(filename.c_str(), &width, &height);
    initSize();
}

void ImageOverlay::initSize() {
    float w, h;

    // Calculate correct scaling
    if (width == 0 || height == 0 || desiredWidth == 0 || desiredHeight == 0) {
        w = 200;
        h = 200;
        std::cerr << "Overlay image size is invalid. Standard scaling will be used." << std::endl;
    } else {
        if (width > height) {
            w = WindowUtils::getInstance().adjustToImageWidth(desiredWidth);
            float scale = w / width;
            h = height * scale;
        } else {
            h = WindowUtils::getInstance().adjustToImageHeight(desiredHeight);
            float scale = h / height;
            w = width * scale;
        }
    } 

    // Set corner positions
    topLeft = ImVec2(center.x - (0.5f * w), center.y - (0.5f * h));
    bottomRight = ImVec2(topLeft.x + w, topLeft.y + h);
}

unsigned int ImageOverlay::loadTextureFromFile(const char* filename, int* outWidth, int* outHeight)
{
    int width, height, channels;
    unsigned char* data = stbi_load(filename, &width, &height, &channels, 4);

    if (!data) {
        std::cerr << "Failed to load image file: " << filename << std::endl;
        return 0;
    }

    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    stbi_image_free(data);

    if (outWidth) *outWidth = width;
    if (outHeight) *outHeight = height;

    return texture;
}

void ImageOverlay::draw() {
    if (texture == 0) {
        return;
    }

    ImDrawList* drawList = ImGui::GetForegroundDrawList();
    ImVec2 imageSize(width, height);
    drawList->AddImage((ImTextureID)(intptr_t)texture, topLeft, bottomRight);
}

void ImageOverlay::setCenterPosition(float x, float y) {
    center = ImVec2(x, y);
}
