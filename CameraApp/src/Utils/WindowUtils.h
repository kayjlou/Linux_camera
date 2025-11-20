/*
* Copyright 2016-2025
* This file is copyrighted by onexip GmbH.
* All rights reserved.
*
* Date:   2025-05-05
* Author: Thorsten Knoelker
*/

#ifndef WINDOW_UTILS_H
#define WINDOW_UTILS_H

#include <atomic>
#include "Constants.h"
#include <imgui.h>

class WindowUtils {
public:
    WindowUtils(const WindowUtils&) = delete;
    WindowUtils& operator=(const WindowUtils&) = delete;
    static WindowUtils& getInstance();
    float adjustToImageWidth(float x) const;
    float adjustToImageHeight(float y) const;
    ImVec2 adjustToImageSize(const ImVec2& vec) const;
    int getWindowWidth() const;
    int getWindowHeight() const;
    void setWindowWidth(int width);
    void setWindowHeight(int height);
private:
    std::atomic<int> windowWidth;
    std::atomic<int> windowHeight;
    float scaleW = 1.0f;
    float scaleH = 1.0f;

    WindowUtils();
    ~WindowUtils();
};

#endif
