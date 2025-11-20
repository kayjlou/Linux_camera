/*
* Copyright 2016-2025
* This file is copyrighted by onexip GmbH.
* All rights reserved.
*
* Date:   2025-05-05
* Author: Thorsten Knoelker
*/

#include "RecordingModeOverlay.h"
#include "Fonts.h"
#include "WindowUtils.h"
#include <iostream>

RecordingModeOverlay::RecordingModeOverlay()
: TimedOverlay(false, shotAppearanceTimeInMilliseconds) {
    textOverlay.setFont(Fonts::getInstance().getRecordingModeFont());
}

void RecordingModeOverlay::init() {
    initPositions();
    updateBackground();
    updateModeText();
}

void RecordingModeOverlay::initPositions() {
    auto windowWidth = Constants::OverlayReferenceWidth;
    float left = (windowWidth - shotRectangleWidth) * 0.5f;
    shotRectangleTopLeft = ImVec2(left, shotRectangleTopOffset);
    shotRectangleBottomRight = ImVec2(windowWidth - left, shotRectangleTopOffset + shotRectangleHeight);

    shotRectangleTopLeft = WindowUtils::getInstance().adjustToImageSize(shotRectangleTopLeft);
    shotRectangleBottomRight = WindowUtils::getInstance().adjustToImageSize(shotRectangleBottomRight);
    roundingScaled = WindowUtils::getInstance().adjustToImageHeight(rounding);
}

void RecordingModeOverlay::draw() {
    if (mode == RecordingMode::None) {
        return;
    }

    if (mode == RecordingMode::Shot) {
        if (!evaluateTimer()) {
            setMode(RecordingMode::None);
            setTimerEnabled(false);
        }
    }

    ImDrawList* drawList = ImGui::GetForegroundDrawList();
    drawList->AddRectFilled(topLeft, bottomRight, backgroundColor, roundingScaled, ImDrawFlags_RoundCornersAll);
    textOverlay.draw();

    if (mode == RecordingMode::Recording) {
        drawRecordingDot();
    } else if (mode == RecordingMode::Shot) {
        drawShotRectangle();
    }
}

void RecordingModeOverlay::setMode(RecordingMode mode) {
    if (this->mode != mode) {
        this->mode = mode;
        updateBackground();
        updateModeText();
    }

    if (mode == RecordingMode::Shot) {
        // Resets timer of shot. It is also possible to capture another frame while the Shot overlay is still visible.
        setTimerEnabled(true);
    }
}

void RecordingModeOverlay::updateModeText() {
    std::string text = "";

    switch (mode) {
        case RecordingMode::Recording:
            text = "RECORDING";
            break;
        case RecordingMode::Shot:
            text = "CAPTURE";
            break;
        default:
            textOverlay.setText(text);
            return;
    }

    textOverlay.setText(text);
    float x = (topLeft.x + bottomRight.x) * 0.5f;
    float y = (topLeft.y + bottomRight.y) * 0.5f;

    if (mode == RecordingMode::Recording) {
        x += WindowUtils::getInstance().adjustToImageHeight(recordingDotRadius);
    }

    textOverlay.setCenter(x, y);
}

void RecordingModeOverlay::updateBackground() {
    float left = 0.0f;
    float right = 0.0f;
    float width;

    switch (mode) {
        case RecordingMode::Recording:
            width = 387.89f;
            left = 1492.0f;
            right = left + width;
            break;
        case RecordingMode::Shot:
            width = 300.0f;
            left = (Constants::OverlayReferenceWidth - width) * 0.5f;
            right = (Constants::OverlayReferenceWidth + width) * 0.5f;
            break;
        default:
            return;
    }

    
    topLeft = ImVec2(left, topOffset);
    bottomRight = ImVec2(right, topOffset + height);

    topLeft = WindowUtils::getInstance().adjustToImageSize(topLeft);
    bottomRight = WindowUtils::getInstance().adjustToImageSize(bottomRight);
}

void RecordingModeOverlay::drawRecordingDot() const {
    const int leftOffset = 32;
    float radius = WindowUtils::getInstance().adjustToImageHeight(recordingDotRadius);
    float x = topLeft.x + WindowUtils::getInstance().adjustToImageWidth(leftOffset) + radius;
    float y = (topLeft.y + bottomRight.y) * 0.5f;
    ImVec2 center(x, y);

    ImDrawList* drawList = ImGui::GetForegroundDrawList();
    drawList->AddCircleFilled(center, radius, circleColor);
}

void RecordingModeOverlay::drawShotRectangle() const {
    ImDrawList* drawList = ImGui::GetForegroundDrawList();
    drawList->AddRect(shotRectangleTopLeft, shotRectangleBottomRight, shotRectangleColor, shotRectangleRounding, 0, shotRectangleThickness);
}
