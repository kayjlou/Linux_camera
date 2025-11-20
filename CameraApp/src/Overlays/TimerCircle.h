/*
* Copyright 2016-2025
* This file is copyrighted by onexip GmbH.
* All rights reserved.
*
* Date:   2025-05-05
* Author: Thorsten Knoelker
*/

#ifndef TIMER_CIRCLE_H
#define TIMER_CIRCLE_H

#include "Overlay.h"
#include "TextOverlay.h"
#include <string>

class TimerCircle : Overlay {
public:
    TimerCircle();
    TimerCircle(double totalTime);
    void draw() override;
    void setCenterPosition(int x, int y);
    void setProgressColor(const ImU32& color);
    void updateProgress(double elapsedTime);
    void setRadius(float radius);
private:
    static constexpr ImU32 emptyColor = IM_COL32(255, 255, 255, 255);
    static constexpr float thickness = 6.0f;

    ImU32 progressColor = IM_COL32(216, 72, 72, 255);
    float radius = 50.0f;
    ImVec2 center = ImVec2(100, 100);
    double totalTime;
    float progress = 0;
    TextOverlay countdown;
};

#endif