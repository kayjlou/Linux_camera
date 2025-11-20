/*
* Copyright 2016-2025
* This file is copyrighted by onexip GmbH.
* All rights reserved.
*
* Date:   2025-05-05
* Author: Thorsten Knoelker
*/

#ifndef BRIGHTNESS_OVERLAY_H
#define BRIGHTNESS_OVERLAY_H

#include "TimedOverlay.h"
#include "TextOverlay.h"
#include "IconOverlay.h"
#include <chrono>

class BrightnessOverlay : public TimedOverlay {
public:
    BrightnessOverlay(std::string headerText = "LED Brightness");
    BrightnessOverlay(unsigned int brightness);
    void draw() override;
    void setBrightness(unsigned int value);
private:
    static constexpr ImU32 backgroundColor = IM_COL32(255, 255, 255, 204);
    static constexpr ImU32 barColor = IM_COL32(14, 153, 208, 255);
    static constexpr ImU32 barBackgroundColor = IM_COL32(255, 255, 255, 255);
    static constexpr int backgroundWidth = 910;
    static constexpr int backgroundHeight = 260;
    static constexpr int backgroundOffsetBottom = 71;
    static constexpr float backgroundRounding = 46.0f;
    static constexpr int barWidth = 663;
    static constexpr int barHeight = 17;
    static constexpr int barOffsetBottom = 165;
    static constexpr float barRounding = 8.35f;
    static constexpr int iconWidth = 47;

    std::string headerText = "LED Brightness";
    ImVec2 backgroundTopLeft;
    ImVec2 backgroundBottomRight;
    ImVec2 barBackgroundTopLeft;
    ImVec2 barBackgroundBottomRight;
    ImVec2 barTopLeft;
    ImVec2 barBottomRight;
    float backgroundRoundingScaled = 46.0f;
    float barRoundingScaled = 8.35f;
    unsigned int brightness = 0;
    TextOverlay header;
    TextOverlay zeroPercentText;
    TextOverlay hundredPercentText;
    IconOverlay sunIcon = IconOverlay(Icon::sun);
    bool iconInitialised = false;

    void initPositions();
    void initTextOverlays();
    void initIcon();
    void drawBar() const;
    void update();
};

#endif
