/*
* Copyright 2016-2025
* This file is copyrighted by onexip GmbH.
* All rights reserved.
*
* Date:   2025-05-05
* Author: Thorsten Knoelker
*/

#ifndef GENERAL_MESSAGE_H
#define GENERAL_MESSAGE_H

#include "IconOverlay.h"
#include "TextOverlay.h"
#include "TimerCircle.h"
#include "TimedOverlay.h"
#include "WindowUtils.h"
#include "Constants.h"
#include <chrono>
#include <string>

class MessageOverlay : public TimedOverlay {
public:
    enum class MessageType {
        info,
        success,
        error
    };

    MessageOverlay(MessageType type, Icon icon, bool temporary, const std::string& headerText, const std::string& text, int appearanceTimeInMilliseconds);
    MessageOverlay(MessageType type, Icon icon, bool temporary, const std::string& headerText, const std::string& text);
    MessageOverlay();
    void init() override;
    void draw() override;
protected:
    bool showTimer = true;
private:
    static constexpr ImU32 backgroundColor = IM_COL32(255, 255, 255, 204);
    static constexpr ImU32 successColor = IM_COL32(66, 208, 14, 255);
    static constexpr ImU32 infoColor = IM_COL32(14, 153, 208, 255);
    static constexpr ImU32 errorColor = IM_COL32(216, 72, 72, 255);

    static constexpr int backgroundOffsetLeft = 505;
    static constexpr int backgroundOffsetTop = 220;
    static constexpr int headerHeight = 236;
    static constexpr int headerTextOffsetTop = 523;
    static constexpr int messageTextOffsetTop = 644;
    static constexpr float rounding = 46.0f;
    static constexpr float timerCircleRadius = 52.0f;

    MessageType messageType;
    ImU32 primaryColor;
    ImVec2 backgroundTopLeft;
    ImVec2 backgroundBottomRight;
    ImVec2 headerTopLeft;
    ImVec2 headerBottomRight;
    IconOverlay iconOverlay;
    TextOverlay header;
    TextOverlay message;
    TimerCircle timerCircle;
    float roundingScaled = 46.0f;

    void initPrimaryColor();
    void initPositions();
    void initTextOverlays();
};

#endif
