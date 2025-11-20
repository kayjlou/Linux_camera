/*
* Copyright 2016-2025
* This file is copyrighted by onexip GmbH.
* All rights reserved.
*
* Date:   2025-05-05
* Author: Thorsten Knoelker
*/

#ifndef CAMERA_H
#define CAMERA_H

#include <atomic>
#include "GmslConnection.h"
#include <chrono>
#include "Constants.h"

class Camera {
public:
    Camera();
    int increaseBrightness();
    int decreaseBrightness();
    void applyAllSettings();
private:
    static constexpr unsigned int minGain = 128;
    static constexpr unsigned int maxGain = 1984;
    static constexpr double brightnessGainConversionFactor = (maxGain - minGain) / 100.0;

    int brightness = Constants::DefaultGain;

    static int convertBrightnessToGain(int brightness);
    void applyBrightness(int value);
    void applyExposure(int valueInMicroseconds);
    void applyHorizontalFlip(bool flip);
    void applyVerticalFlip(bool flip);
};

#endif
