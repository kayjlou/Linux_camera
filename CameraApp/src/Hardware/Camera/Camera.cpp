/*
* Copyright 2016-2025
* This file is copyrighted by onexip GmbH.
* All rights reserved.
*
* Date:   2025-05-05
* Author: Thorsten Knoelker
*/

#include "Camera.h"
#include "CameraRegisterAccess.h"
#include "MessageHandler.h"
#include "Utils.h"
#include <cmath>

Camera::Camera() {
}

int Camera::increaseBrightness() {
    if (brightness < 100) {
        int newValue = brightness + Constants::BrightnessStep;

        if (newValue > 100) {
            newValue = 100;
        }

        applyBrightness(newValue);
    }

    return brightness;
}

int Camera::decreaseBrightness() {
    if (brightness > 0) {
        int newValue = brightness - Constants::BrightnessStep;

        if (newValue < 0) {
            newValue = 0;
        }

        applyBrightness(newValue);
    }

    return brightness;
}

void Camera::applyBrightness(int value) {
    #ifndef SIMULATE_ALL_HARDWARE
    int gain = convertBrightnessToGain(value);
    int ret = CameraRegisterAccess::getInstance().write16bitValue(0x3508, 0x3509, gain);

    if (ret == RET_FAIL) {
        return;
    }
    #endif

    brightness = value;
}

int Camera::convertBrightnessToGain(int brightness) {
    double gain = brightness * brightnessGainConversionFactor + minGain;

    return static_cast<unsigned int>(std::round(gain));
}

void Camera::applyExposure(int valueInMicroseconds) {
    int ret = CameraRegisterAccess::getInstance().writeRegister(0x3500, (valueInMicroseconds >> 16) & 0xff);

    if (ret == RET_FAIL) {
        return;
    }

    ret = CameraRegisterAccess::getInstance().writeRegister(0x3501, (valueInMicroseconds >> 8) & 0xff);

    if (ret == RET_FAIL) {
        return;
    }

    ret = CameraRegisterAccess::getInstance().writeRegister(0x3502, valueInMicroseconds & 0xff);

    if (ret == RET_FAIL) {
        return;
    }
}

void Camera::applyHorizontalFlip(bool flip) {
    u8 value;
    int ret = CameraRegisterAccess::getInstance().readRegister(0x3821, &value);

    if (ret == RET_FAIL) {
        return;
    }

    if (flip) {
        value |= 0x06;
    } else {
        value &= 0xF9;
    }

    CameraRegisterAccess::getInstance().writeRegister(0x3821, value);
}

void Camera::applyVerticalFlip(bool flip) {
    u8 value;
    int ret = CameraRegisterAccess::getInstance().readRegister(0x3820, &value);

    if (ret == RET_FAIL) {
        return;
    }

    if (flip) {
        value |= 0x06;
    } else {
        value &= 0xF9;
    }

    CameraRegisterAccess::getInstance().writeRegister(0x3820, value);
}

void Camera::applyAllSettings() {
    applyBrightness(brightness);
    applyExposure(Constants::DefaultExposureTimeInMs * 1000);
    applyHorizontalFlip(false);
    applyVerticalFlip(false);
}
