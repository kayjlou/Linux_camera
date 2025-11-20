/*
* Copyright 2016-2025
* This file is copyrighted by onexip GmbH.
* All rights reserved.
*
* Date:   2025-08-13
* Author: Thorsten Knoelker
*/

#ifndef ACTIVE_CAMERA_OVERLAY_H
#define ACTIVE_CAMERA_OVERLAY_H

#include "Overlay.h"
#include "TextOverlay.h"
#include <chrono>
#include <atomic>

class ActiveCameraOverlay : public Overlay {
public:
    ActiveCameraOverlay();
    void draw() override;
    void init() override;
    int getActiveCamera();
    void setActiveCamera(int cameraId);
    void setActiveCamera(int cameraId, bool connected);
    void setCameraAvailability(int cameraId, bool available);
    void setSwitchingInProgress();
private:
    static constexpr ImU32 colorActive = IM_COL32(50, 255, 50, 180);
    static constexpr ImU32 colorInactive = IM_COL32(255, 255, 255, 153);
    static constexpr ImU32 textColorActive = IM_COL32(0, 0, 0, 255);
    static constexpr ImU32 textColorInactive = IM_COL32(128, 128, 128, 255);
    static constexpr float rounding = 47.0f;
    static constexpr float bottomOffset = 47.0f;
    static constexpr float leftRightOffset = 72.0f;
    static constexpr float width = 170.0f;
    static constexpr float height = 78.0f;
    static constexpr int flashFrequencyInMilliseconds = 200;

    ImVec2 cam1TopLeft;
    ImVec2 cam1BottomRight;
    ImVec2 cam2TopLeft;
    ImVec2 cam2BottomRight;
    float roundingScaled = 47.0f;
    TextOverlay cam1TextOverlay;
    TextOverlay cam2TextOverlay;
    int activeCamera = 1; // 0 = both inactive, 1 = cam1 active, 2 = cam2 active
    std::atomic<bool> cam1Available{false};
    std::atomic<bool> cam2Available{false};
    ImU32 cam1Color = colorInactive;
    ImU32 cam2Color = colorInactive;
    bool switchingInProgress = false;
    std::chrono::time_point<std::chrono::high_resolution_clock> switchingStartTime;

    void initBackgroundPositions();
    void initTextOverlays();
    void updateSwitchingColor();
};

#endif
