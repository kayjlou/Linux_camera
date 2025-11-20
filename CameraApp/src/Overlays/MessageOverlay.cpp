/*
* Copyright 2016-2025
* This file is copyrighted by onexip GmbH.
* All rights reserved.
*
* Date:   2025-05-05
* Author: Thorsten Knoelker
*/

#include "MessageOverlay.h"
#include "Fonts.h"

MessageOverlay::MessageOverlay(MessageType type, Icon icon, bool temporary, const std::string& headerText, const std::string& text, int appearanceTimeInMilliseconds)
: TimedOverlay(temporary, appearanceTimeInMilliseconds), timerCircle(appearanceTimeInMilliseconds) {
    this->messageType = type;
    this->iconOverlay = IconOverlay(icon);
    header.setText(headerText);
    message.setText(text);
    setExpirationCheckFrequency(0);
    initPrimaryColor();
    initPositions();
    initTextOverlays();
}

MessageOverlay::MessageOverlay(MessageType type, Icon icon, bool temporary, const std::string& headerText, const std::string& text)
: MessageOverlay(type, icon, temporary, headerText, text, Constants::MessageAppearanceTimeInMilliseconds) {
}

MessageOverlay::MessageOverlay() : MessageOverlay(MessageType::info, Icon::none, true, "", "") {
}

void MessageOverlay::init() {
    iconOverlay.init();
}

void MessageOverlay::initPrimaryColor() {
    switch (messageType) {
        case MessageType::info:
            primaryColor = infoColor;
            break;
        case MessageType::success:
            primaryColor = successColor;
            break;
        case MessageType::error:
            primaryColor = errorColor;
            break;
        default:
            primaryColor = infoColor;
            break;
    }

    timerCircle.setProgressColor(primaryColor);
}

void MessageOverlay::initPositions() {
    auto windowWidth = Constants::OverlayReferenceWidth;
    auto windowHeight = Constants::OverlayReferenceHeight;

    backgroundTopLeft = ImVec2(backgroundOffsetLeft, backgroundOffsetTop);
    backgroundBottomRight = ImVec2(windowWidth - backgroundOffsetLeft, windowHeight - backgroundOffsetTop);
    headerTopLeft = backgroundTopLeft;
    headerBottomRight = ImVec2(backgroundBottomRight.x, headerTopLeft.y + headerHeight);

    backgroundTopLeft = WindowUtils::getInstance().adjustToImageSize(backgroundTopLeft);
    backgroundBottomRight = WindowUtils::getInstance().adjustToImageSize(backgroundBottomRight);
    headerTopLeft = WindowUtils::getInstance().adjustToImageSize(headerTopLeft);
    headerBottomRight = WindowUtils::getInstance().adjustToImageSize(headerBottomRight);

    iconOverlay.setCenterPosition((headerTopLeft.x + headerBottomRight.x) * 0.5f, (headerTopLeft.y + headerBottomRight.y) * 0.5f);

    float radius = WindowUtils::getInstance().adjustToImageWidth(timerCircleRadius);
    timerCircle.setCenterPosition(backgroundBottomRight.x - 2*radius, backgroundBottomRight.y - 2*radius);
    timerCircle.setRadius(radius);

    roundingScaled = WindowUtils::getInstance().adjustToImageHeight(rounding);
}

void MessageOverlay::initTextOverlays() {
    header.setFont(Fonts::getInstance().getHeaderFont());
    message.setFont(Fonts::getInstance().getMessageFont());

    float x = WindowUtils::getInstance().getWindowWidth() * 0.5f;
    float yHeader = WindowUtils::getInstance().adjustToImageHeight(headerTextOffsetTop);
    header.setCenter(x, yHeader);
    
    float yMessage = WindowUtils::getInstance().adjustToImageHeight(messageTextOffsetTop);
    message.setCenter(x, yMessage);
}

void MessageOverlay::draw() {
    if (!evaluateTimer()) {
        return;
    }

    ImDrawList* drawList = ImGui::GetForegroundDrawList();
    drawList->AddRectFilled(backgroundTopLeft, backgroundBottomRight, backgroundColor, roundingScaled, ImDrawFlags_RoundCornersAll);
    drawList->AddRectFilled(headerTopLeft, headerBottomRight, primaryColor, roundingScaled, ImDrawFlags_RoundCornersAll);
    header.draw();
    message.draw();
    iconOverlay.draw();

    if (showTimer && getTimerEnabled()) {
        timerCircle.updateProgress(getElapsedTime());
        timerCircle.draw();
    }
}
