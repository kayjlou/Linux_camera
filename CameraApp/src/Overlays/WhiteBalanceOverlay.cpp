/*
* Copyright 2016-2025
* This file is copyrighted by onexip GmbH.
* All rights reserved.
*
* Date:   2025-05-05
* Author: Thorsten Knoelker
*/

#include "WhiteBalanceOverlay.h"
#include "WindowUtils.h"
#include "Fonts.h"
#include "WhiteBalance.h"

WhiteBalanceOverlay::WhiteBalanceOverlay() {
    initPositions();
    initTextOverlays();
    progress = 0.0;
}

void WhiteBalanceOverlay::initPositions() {
    auto windowWidth = Constants::OverlayReferenceWidth;
    auto windowHeight = Constants::OverlayReferenceHeight;

    float backgroundOffsetLeft = (windowWidth - backgroundWidth) * 0.5f;
    float barOffsetLeft = (windowWidth - barWidth) * 0.5f;

    backgroundTopLeft = ImVec2(backgroundOffsetLeft, windowHeight - backgroundOffsetBottom - backgroundHeight);
    backgroundBottomRight = ImVec2(windowWidth - backgroundOffsetLeft, windowHeight - backgroundOffsetBottom);
    barBackgroundTopLeft = ImVec2(barOffsetLeft, windowHeight - barOffsetBottom - barHeight);
    barBackgroundBottomRight = ImVec2(windowWidth - barOffsetLeft, windowHeight - barOffsetBottom);
    barTopLeft = barBackgroundTopLeft;
    barBottomRight = ImVec2(barTopLeft.x, barBackgroundBottomRight.y);

    backgroundTopLeft = WindowUtils::getInstance().adjustToImageSize(backgroundTopLeft);
    backgroundBottomRight = WindowUtils::getInstance().adjustToImageSize(backgroundBottomRight);
    barBackgroundTopLeft = WindowUtils::getInstance().adjustToImageSize(barBackgroundTopLeft);
    barBackgroundBottomRight = WindowUtils::getInstance().adjustToImageSize(barBackgroundBottomRight);
    barTopLeft = WindowUtils::getInstance().adjustToImageSize(barTopLeft);
    barBottomRight = WindowUtils::getInstance().adjustToImageSize(barBottomRight);

    backgroundRoundingScaled = WindowUtils::getInstance().adjustToImageHeight(backgroundRounding);
    barRoundingScaled = WindowUtils::getInstance().adjustToImageHeight(barRounding);
}

void WhiteBalanceOverlay::initTextOverlays() {
    text.setFont(Fonts::getInstance().getHeaderFont());
    float x = (backgroundTopLeft.x + backgroundBottomRight.x) * 0.5f;
    float y = backgroundTopLeft.y + WindowUtils::getInstance().adjustToImageHeight(55);
    text.setCenter(x, y);
    text.setText("White Balance");
}

void WhiteBalanceOverlay::draw() {
    ImDrawList* drawList = ImGui::GetForegroundDrawList();
    drawList->AddRectFilled(backgroundTopLeft, backgroundBottomRight, backgroundColor, backgroundRoundingScaled, ImDrawFlags_RoundCornersAll);
    text.draw();
    drawBar();
}

void WhiteBalanceOverlay::drawBar() {
    // Update bar according to elapsed time
    barBottomRight.x = barBackgroundTopLeft.x + ((barBackgroundBottomRight.x - barBackgroundTopLeft.x) * progress);

    ImDrawList* drawList = ImGui::GetForegroundDrawList();
    drawList->AddRectFilled(barBackgroundTopLeft, barBackgroundBottomRight, barBackgroundColor, barRoundingScaled, ImDrawFlags_RoundCornersAll);
    drawList->AddRectFilled(barTopLeft, barBottomRight, barColor, barRoundingScaled, ImDrawFlags_RoundCornersAll);
}

void WhiteBalanceOverlay::updateProgress(double progress) {
    this->progress = progress;
}
