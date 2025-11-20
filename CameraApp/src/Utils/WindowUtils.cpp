/*
* Copyright 2016-2025
* This file is copyrighted by onexip GmbH.
* All rights reserved.
*
* Date:   2025-05-05
* Author: Thorsten Knoelker
*/

#include "WindowUtils.h"
#include <numeric>
#include <cmath>

WindowUtils& WindowUtils::getInstance() {
    static WindowUtils instance;
    return instance;
}

WindowUtils::WindowUtils() : windowWidth(1920), windowHeight(1080) {
}

WindowUtils::~WindowUtils() {
}

int WindowUtils::getWindowWidth() const {
    return windowWidth;
}

int WindowUtils::getWindowHeight() const {
    return windowHeight;
}

void WindowUtils::setWindowWidth(int width) {
    this->windowWidth = width;
    scaleW = width / Constants::OverlayReferenceWidth;
}

void WindowUtils::setWindowHeight(int height) {
    this->windowHeight = height;
    scaleH = height / Constants::OverlayReferenceHeight;
}

float WindowUtils::adjustToImageWidth(float x) const {
    return x * scaleW;
}

float WindowUtils::adjustToImageHeight(float y) const {
    return y * scaleH;
}

ImVec2 WindowUtils::adjustToImageSize(const ImVec2& vec) const {
    return ImVec2(vec.x * scaleW, vec.y * scaleH);
}
