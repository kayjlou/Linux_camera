/*
* Copyright 2016-2025
* This file is copyrighted by onexip GmbH.
* All rights reserved.
*
* Date:   2025-05-09
* Author: Thorsten Knoelker
*/

#ifndef MESSAGE_STRINGS_H
#define MESSAGE_STRINGS_H

#include <string>

class MessageStrings {
public:
    inline static const std::string CameraConnectedHeader = "Camera is connected";
    inline static const std::string CameraConnectedText = "Everything is ready to go!\nYou can start using the camera now.";
    inline static const std::string CameraDisconnectedHeader = "Camera not connected";
    inline static const std::string CameraDisconnectedText = "Ensure the camera is properly\nconnected and powered on, then\nattempt to reconnect";
    inline static const std::string FrozenFrameHeader = "Camera stream is frozen";
    inline static const std::string FrozenFrameText = "No new images from camera can be received.";
    inline static const std::string NoUsbStorageHeader = "No USB storage connected";
    inline static const std::string NoUsbStorageText = "Connect USB storage to save videos and images.";
    inline static const std::string RecordingAboutToStartHeader = "Recording is starting...";
    inline static const std::string RecordingAboutToStartText = "Ensure the patient is positioned\ncorrectly and remains still. Recording\nwill start shortly.";
    inline static const std::string SavingVideoHeader = "Finishing video saving";
    inline static const std::string SavingVideoText = "Saving video file to USB storage...";
    inline static const std::string SavingVideoSuccessfulHeader = "Video recording successful";
    inline static const std::string SavingVideoSuccessfulText = "Recorded video was saved to USB storage.";
    inline static const std::string SavingVideoFailedHeader = "Video recording failed";
    inline static const std::string SavingVideoFailedText = "Saving video to USB storage failed.";
    inline static const std::string TemperatureWarningHeader = "Temperature high";
    inline static const std::string TemperatureWarningText = "The temperature is too high. LED intensity has been reduced.";
    inline static const std::string TemperatureCriticalHeader = "Temperature critical";
    inline static const std::string TemperatureCriticalText = "The temperature is critically high.\nLEDs haven been switched off.";
    inline static const std::string UartTimeoutHeader = "UART Timeout";
    inline static const std::string UartTimeoutText = "No UART messages received within the expected time.";
};

#endif
