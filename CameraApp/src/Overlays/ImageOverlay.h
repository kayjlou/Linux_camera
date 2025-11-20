/*
* Copyright 2016-2025
* This file is copyrighted by onexip GmbH.
* All rights reserved.
*
* Date:   2025-05-05
* Author: Thorsten Knoelker
*/

#ifndef IMAGE_OVERLAY_H
#define IMAGE_OVERLAY_H

#include "stb_image.h"

#include <string>
#include "Overlay.h"

#ifdef __APPLE__
    #include <OpenGL/gl3.h>  // macOS uses OpenGL framework
#else
    #ifdef USE_GLES
        #include <gles2.h>
    #else
        #include <gl.h>
    #endif
#endif

class ImageOverlay : Overlay {
public:
    ImageOverlay(const std::string& filename);
    void init() override;
    void draw() override;
    void setCenterPosition(float x, float y);
    void setSize(int desiredWidth, int desiredHeight);
private:
    std::string filename;
    unsigned int texture = 0;
    int width;
    int height;
    ImVec2 center = ImVec2(200, 200);
    ImVec2 topLeft;
    ImVec2 bottomRight;
    int desiredWidth = 200;
    int desiredHeight = 200;

    static unsigned int loadTextureFromFile(const char* filename, int* outWidth, int* outHeight);
    void initSize();
};

#endif
