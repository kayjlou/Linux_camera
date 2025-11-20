/*
* Copyright 2016-2025
* This file is copyrighted by onexip GmbH.
* All rights reserved.
*
* Date:   2025-08-13
* Author: Thorsten Knoelker
*/

#include "ActiveCameraOverlay.h"
#include "Fonts.h"
#include "WindowUtils.h"
#include "Utils.h"
#include <iostream>

ActiveCameraOverlay::ActiveCameraOverlay() {
    cam1TextOverlay.setFont(Fonts::getInstance().getRecordingModeFont());
    cam2TextOverlay.setFont(Fonts::getInstance().getRecordingModeFont());
}

void ActiveCameraOverlay::init() {
    initBackgroundPositions();
    initTextOverlays();
}

void ActiveCameraOverlay::initBackgroundPositions() {
    cam1TopLeft = ImVec2(leftRightOffset, Constants::OverlayReferenceHeight - bottomOffset - height);
    cam1BottomRight = ImVec2(leftRightOffset + width, Constants::OverlayReferenceHeight - bottomOffset);

    cam2TopLeft = ImVec2(Constants::OverlayReferenceWidth - leftRightOffset - width, Constants::OverlayReferenceHeight - bottomOffset - height);
    cam2BottomRight = ImVec2(Constants::OverlayReferenceWidth - leftRightOffset, Constants::OverlayReferenceHeight - bottomOffset);

    cam1TopLeft = WindowUtils::getInstance().adjustToImageSize(cam1TopLeft);
    cam1BottomRight = WindowUtils::getInstance().adjustToImageSize(cam1BottomRight);
    cam2TopLeft = WindowUtils::getInstance().adjustToImageSize(cam2TopLeft);
    cam2BottomRight = WindowUtils::getInstance().adjustToImageSize(cam2BottomRight);
    roundingScaled = WindowUtils::getInstance().adjustToImageHeight(rounding);
}

void ActiveCameraOverlay::initTextOverlays() {
    cam1TextOverlay.setFontColor(textColorActive);
    cam2TextOverlay.setFontColor(textColorInactive);

    cam1TextOverlay.setCenter((cam1TopLeft.x + cam1BottomRight.x) * 0.5f, (cam1TopLeft.y + cam1BottomRight.y) * 0.5f);
    cam2TextOverlay.setCenter((cam2TopLeft.x + cam2BottomRight.x) * 0.5f, (cam2TopLeft.y + cam2BottomRight.y) * 0.5f);
    cam1TextOverlay.setText("CAM 1");
    cam2TextOverlay.setText("CAM 2");
}

void ActiveCameraOverlay::draw() {
    ImDrawList* drawList = ImGui::GetForegroundDrawList();

    updateSwitchingColor();

    if (cam1Available) {
        drawList->AddRectFilled(cam1TopLeft, cam1BottomRight, cam1Color, roundingScaled, ImDrawFlags_RoundCornersAll);
        cam1TextOverlay.draw();
    }

    if (cam2Available) {
        drawList->AddRectFilled(cam2TopLeft, cam2BottomRight, cam2Color, roundingScaled, ImDrawFlags_RoundCornersAll);
        cam2TextOverlay.draw();
    }
}

void ActiveCameraOverlay::updateSwitchingColor() {
    if (switchingInProgress) {
        auto timePassed = Utils::timePassedSince(switchingStartTime);
        auto value = timePassed % (2*flashFrequencyInMilliseconds);
        ImU32 currentColor;

        if (value < flashFrequencyInMilliseconds) {
            currentColor = colorInactive;
        } else {
            currentColor = colorActive;
        }

        // The overlay of the camera that is currently not active will flash
        if (activeCamera == 1) {
            cam2Color = currentColor;
        } else if (activeCamera == 2) {
            cam1Color = currentColor;
        }
    }
}

void ActiveCameraOverlay::setActiveCamera(int cameraId) {
    switchingInProgress = false;
    activeCamera = cameraId;

    if (cameraId == 0) {
        cam1Color = colorInactive;
        cam2Color = colorInactive;
        cam1TextOverlay.setFontColor(textColorInactive);
        cam2TextOverlay.setFontColor(textColorInactive);
    } else if (cameraId == 1) {
        cam1Color = colorActive;
        cam2Color = colorInactive;
        cam1TextOverlay.setFontColor(textColorActive);
        cam2TextOverlay.setFontColor(textColorInactive);
    } else if (cameraId == 2) {
        cam1Color = colorInactive;
        cam2Color = colorActive;
        cam1TextOverlay.setFontColor(textColorInactive);
        cam2TextOverlay.setFontColor(textColorActive);
    }
}

void ActiveCameraOverlay::setActiveCamera(int cameraId, bool connected) {
    if (connected) {
        activeCamera = cameraId;
    } else {
        activeCamera = 0;
    }

    setActiveCamera(cameraId);
}

int ActiveCameraOverlay::getActiveCamera() {
    return activeCamera;
}

void ActiveCameraOverlay::setCameraAvailability(int cameraId, bool available) {
    if (cameraId == 1) {
        cam1Available = available;
    } else if (cameraId == 2) {
        cam2Available = available;
    }
}

void ActiveCameraOverlay::setSwitchingInProgress() {
    switchingInProgress = true;
    switchingStartTime = std::chrono::high_resolution_clock::now();
    cam1Color = colorInactive;
    cam2Color = colorInactive;
    cam1TextOverlay.setFontColor(textColorInactive);
    cam2TextOverlay.setFontColor(textColorInactive);
}
