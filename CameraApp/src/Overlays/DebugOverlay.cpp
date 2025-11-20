/*
* Copyright 2016-2025
* This file is copyrighted by onexip GmbH.
* All rights reserved.
*
* Date:   2025-05-12
* Author: Thorsten Knoelker
*/

#include "DebugOverlay.h"
#include "Fonts.h"
#include "WindowUtils.h"

DebugOverlay::DebugOverlay() : TimedOverlay(true, AppearanceTimeInMilliseconds) {
}

void DebugOverlay::init() {
    textOverlay.setFont(Fonts::getInstance().getHeaderFont());
    textOverlay.setFontColor(textColor);

    float x = Constants::CameraImageWidth * 0.5f;
    float y = Constants::OverlayReferenceHeight - 60;
    x = WindowUtils::getInstance().adjustToImageWidth(x);
    y = WindowUtils::getInstance().adjustToImageHeight(y);
    textOverlay.setCenter(x, y);
}

void DebugOverlay::draw() {
    if (!evaluateTimer()) {
        return;
    }

    textOverlay.draw();
}

void DebugOverlay::setText(const std::string& text) {
    textOverlay.setText(text);
    this->resetTimer();
}
