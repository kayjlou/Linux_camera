/*
* Copyright 2016-2025
* This file is copyrighted by onexip GmbH.
* All rights reserved.
*
* Date:   2025-05-05
* Author: Thorsten Knoelker
*/

#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <atomic>
#include <chrono>

class Constants {
public:
    // Display dimensions
    static constexpr int CameraImageWidth = 1920;
    static constexpr int CameraImageHeight = 1080;
    static constexpr float OverlayReferenceWidth = 1920.0f;
    static constexpr float OverlayReferenceHeight = 1080.0f;

    // Message appearance times
    static constexpr int MessageAppearanceTimeInMilliseconds = 5000;
    static constexpr int CameraConnectedAppearanceTimeInMilliseconds = 3000;
    static constexpr int NoUsbStorageAppearanceTimeInMilliseconds = 3000;
    static constexpr int RecordingDelayTimeInMilliseconds = 5000;
    static constexpr int SavingVideoSuccessfulAppearanceTimeInMilliseconds = 2000;
    static constexpr int SavingVideoFailedAppearanceTimeInMilliseconds = 2000;

    // I2C communication
    static constexpr int i2cCommunicationDelayInMilliseconds = 200;

    // GMSL connection
    static constexpr int MaximumNumberOfRetries = 3000; // The maximum of attempts to connect to a camera
    static constexpr std::chrono::milliseconds ReconnectionWaitTime = std::chrono::milliseconds(200); // The wait time between two connection attempts
    static constexpr std::chrono::milliseconds ConnectionCheckInterval = std::chrono::milliseconds(500); // The interval it is checked whether the GMSL connection is still active

    // Camera settings
    static constexpr int DefaultGain = 80;
    static constexpr int DefaultExposureTimeInMs = 50;  // 50ms corresponds to 20 fps
    static constexpr int BrightnessStep = 10;

    // White balance calibration
    static constexpr int WhiteBalanceCalibrationTotalFrames = 20;
    static constexpr int WhiteBalanceCalibrationSkipFrames = 5;

    // Distal LED control
    static constexpr int MaximumLedValue = 25;
    static constexpr int MinimumLedValue = 0;
    static constexpr int ReducedLedValue = 5;
    static constexpr int LedStep = 1;
    static constexpr int DefaultLedValue = 20;

    // Temperature monitoring
    static constexpr float WarningTemperature = 70.0f;
    static constexpr float CriticalTemperature = 80.0f;
    static constexpr std::chrono::milliseconds ReadTemperatureInterval = std::chrono::milliseconds(1000);

    // Video freeze detection
    static constexpr unsigned int PtsEqualCountThreshold = 20;
    static constexpr unsigned int SampleRenderCountThreshold = 90; // How many times does the same frame get rendered until we consider the video frozen
    static constexpr int FrozenTimeThreshold = 4000; // Wait for this time after video freeze detection before triggering a camera reconnection

    // Font sizes
    static constexpr float HeaderFontSize = 48.0f;
    static constexpr float MessageFontSize = 36.0f;
    static constexpr float RecordingModeFontSize = 48.0f;
    static constexpr float CountdownFontSize = 52.0f;
    static constexpr float SwitchFontSize = 23.82f;
};

#endif
