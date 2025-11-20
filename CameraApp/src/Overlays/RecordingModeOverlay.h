/*
* Copyright 2016-2025
* This file is copyrighted by onexip GmbH.
* All rights reserved.
*
* Date:   2025-05-05
* Author: Thorsten Knoelker
*/

#ifndef RECORDING_MODE_OVERLAY_H
#define RECORDING_MODE_OVERLAY_H

#include "TimedOverlay.h"
#include "TextOverlay.h"
#include <chrono>

class RecordingModeOverlay : public TimedOverlay {
public:
    enum class RecordingMode {
        None,
        Recording,
        Shot
    };

    RecordingModeOverlay();
    void draw() override;
    void init() override;
    void setMode(RecordingMode mode);
    void initPositions();
private:
    static constexpr double shotAppearanceTimeInMilliseconds = 2000.0f;
    static constexpr float rounding = 47.0f;
    static constexpr ImU32 backgroundColor = IM_COL32(255, 255, 255, 153);
    static constexpr ImU32 circleColor = IM_COL32(216, 72, 72, 255);
    static constexpr ImU32 shotRectangleColor = IM_COL32(255, 255, 255, 64);
    static constexpr float shotRectangleRounding = 38.0f;
    static constexpr float shotRectangleThickness = 4.0f;
    static constexpr int shotRectangleWidth = 1500;
    static constexpr int shotRectangleHeight = 860;
    static constexpr int shotRectangleTopOffset = 180;
    static constexpr int recordingDotRadius = 14;
    static constexpr float topOffset = 47.0f;
    static constexpr float height = 78.0f;

    TextOverlay textOverlay;
    RecordingMode mode = RecordingMode::None;
    ImVec2 topLeft;
    ImVec2 bottomRight;
    ImVec2 shotRectangleTopLeft;
    ImVec2 shotRectangleBottomRight;
    float roundingScaled = 47.0f;

    void updateModeText();
    void updateBackground();
    void drawRecordingDot() const;
    void drawShotRectangle() const;
};

#endif
