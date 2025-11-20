/*
* Copyright 2016-2025
* This file is copyrighted by onexip GmbH.
* All rights reserved.
*
* Date:   2025-05-05
* Author: Thorsten Knoelker
*/

#include "BrightnessOverlay.h"
#include "WindowUtils.h"
#include "Fonts.h"

BrightnessOverlay::BrightnessOverlay(std::string headerText) {
    this->headerText = headerText;
    initPositions();
    initTextOverlays();
}

BrightnessOverlay::BrightnessOverlay(unsigned int brightness) : BrightnessOverlay() {
    setBrightness(brightness);
}

void BrightnessOverlay::initPositions() {
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

void BrightnessOverlay::initTextOverlays() {
    header.setFont(Fonts::getInstance().getHeaderFont());
    header.setCenter(WindowUtils::getInstance().getWindowWidth() * 0.5f, backgroundTopLeft.y + WindowUtils::getInstance().adjustToImageHeight(55));
    header.setText(headerText);

    float y = (barBackgroundTopLeft.y + barBackgroundBottomRight.y) * 0.5f;

    zeroPercentText.setFont(Fonts::getInstance().getMessageFont());
    zeroPercentText.setCenter((backgroundTopLeft.x + barBackgroundTopLeft.x) * 0.5f, y);
    zeroPercentText.setText("0%");

    hundredPercentText.setFont(Fonts::getInstance().getMessageFont());
    hundredPercentText.setCenter((backgroundBottomRight.x + barBackgroundBottomRight.x) * 0.5f, y);
    hundredPercentText.setText("100%");
}

void BrightnessOverlay::setBrightness(unsigned int value) {
    if (value > 100) {
        value = 100;
    }

    brightness = value;
    update();
}

void BrightnessOverlay::update() {
    // Update bar according to brightness value
    barBottomRight.x = barBackgroundTopLeft.x + ((barBackgroundBottomRight.x - barBackgroundTopLeft.x) * brightness / 100.0f);
    resetTimer();
}

void BrightnessOverlay::draw() {
    if (!evaluateTimer()) {
        return;
    }

    ImDrawList* drawList = ImGui::GetForegroundDrawList();
    drawList->AddRectFilled(backgroundTopLeft, backgroundBottomRight, backgroundColor, backgroundRoundingScaled, ImDrawFlags_RoundCornersAll);
    drawBar();

    header.draw();
    zeroPercentText.draw();
    hundredPercentText.draw();

    if (!iconInitialised) {
        initIcon();
    }

    sunIcon.draw();
}

void BrightnessOverlay::initIcon() {
    sunIcon.setSize(iconWidth, iconWidth);
    auto topLeft = header.getTopLeft();
    auto center = header.getCenter();
    sunIcon.setCenterPosition(topLeft.x - iconWidth * 0.5f, center.y);
    sunIcon.init();
    header.setCenter(center.x + iconWidth * 0.5f, center.y); // Re-adjust header position
    iconInitialised = true;
}

void BrightnessOverlay::drawBar() const {
    ImDrawList* drawList = ImGui::GetForegroundDrawList();

    if (brightness == 100) {
        // Draw only foreground bar
        drawList->AddRectFilled(barBackgroundTopLeft, barBackgroundBottomRight, barColor, barRoundingScaled, ImDrawFlags_RoundCornersAll);
        return;
    }

    drawList->AddRectFilled(barBackgroundTopLeft, barBackgroundBottomRight, barBackgroundColor, barRoundingScaled, ImDrawFlags_RoundCornersAll);

    if (brightness > 0) {
        drawList->AddRectFilled(barTopLeft, barBottomRight, barColor, barRoundingScaled, ImDrawFlags_RoundCornersAll);
    }
}
