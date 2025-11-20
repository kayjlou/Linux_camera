/*
* Copyright 2016-2025
* This file is copyrighted by onexip GmbH.
* All rights reserved.
*
* Date:   2025-05-05
* Author: Thorsten Knoelker
*/

#ifndef WHITE_BALANCE_OVERLAY_H
#define WHITE_BALANCE_OVERLAY_H

#include "TextOverlay.h"
#include <atomic>

class WhiteBalanceOverlay : public Overlay {
public:
    WhiteBalanceOverlay();
    void draw() override;
    bool expired() const;
    void updateProgress(double progress);
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

    ImVec2 backgroundTopLeft;
    ImVec2 backgroundBottomRight;
    ImVec2 barBackgroundTopLeft;
    ImVec2 barBackgroundBottomRight;
    ImVec2 barTopLeft;
    ImVec2 barBottomRight;
    TextOverlay text;
    float backgroundRoundingScaled = 46.0f;
    float barRoundingScaled = 8.35f;
    std::atomic<double> progress;

    void initPositions();
    void initTextOverlays();
    void drawBar();
};

#endif
