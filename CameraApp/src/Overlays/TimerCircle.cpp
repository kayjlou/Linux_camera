/*
* Copyright 2016-2025
* This file is copyrighted by onexip GmbH.
* All rights reserved.
*
* Date:   2025-05-05
* Author: Thorsten Knoelker
*/

#include "TimerCircle.h"
#include "Constants.h"
#include "Fonts.h"
#include <cmath>

TimerCircle::TimerCircle() : TimerCircle(Constants::MessageAppearanceTimeInMilliseconds) {
}

TimerCircle::TimerCircle(double totalTime) {
    this->totalTime = totalTime;
    countdown.setFont(Fonts::getInstance().getCountdownFont());
}

void TimerCircle::draw() {
    ImDrawList* drawList = ImGui::GetForegroundDrawList();

    if (progress >= 1) {
        drawList->AddCircle(center, radius, progressColor, 0, thickness);    
    } else {
        drawList->AddCircle(center, radius, emptyColor, 0, thickness);
    
        // Draw progress circle
        drawList->PathClear();
        float endAngle = M_PI * 2.0f * progress;
        drawList->PathArcTo(center, radius, 0, endAngle, 100);
        drawList->PathStroke(progressColor, false, thickness + 1.0);
    }

    countdown.draw();
}

void TimerCircle::setCenterPosition(int x, int y) {
    center = ImVec2(x, y);
    countdown.setCenter(x, y);
}

void TimerCircle::setProgressColor(const ImU32& color) {
    progressColor = color;
}

void TimerCircle::updateProgress(double elapsedTime) {
    progress = elapsedTime / totalTime;

    if (progress >= 1) {
        countdown.setText("0");    
    } else {
        double secondsRemaining = (totalTime - elapsedTime) / 1000.0;
        int rounded = std::round(secondsRemaining);
        countdown.setText(std::to_string(rounded));
    }
}

void TimerCircle::setRadius(float radius) {
    this->radius = radius;
} 
