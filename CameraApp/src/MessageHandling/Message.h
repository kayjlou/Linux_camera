/*
* Copyright 2016-2025
* This file is copyrighted by onexip GmbH.
* All rights reserved.
*
* Date:   2025-05-05
* Author: Thorsten Knoelker
*/

#ifndef MESSAGE_H
#define MESSAGE_H

#include "MessageOverlay.h"
#include <atomic>

class Message {
public:
    enum class MessageType {
        CameraConnectedInitially,
        CameraConnected,
        CameraDisconnected,
        FrozenFrame,
        NoUsbStorage,
        RecordingAboutToStart,
        SavingVideo,
        SavingVideoSuccessful,
        SavingVideoFailed,
        WhiteBalance,
        Gain,
        LedBrightness,
        TemperatureWarning,
        TemperatureCritical,
        UartTimeout,
    };

    Message(MessageType messageType);
    ~Message();

    MessageType getType() const;
    int getPriority() const;
    bool isExpired() const;
    void updateValue(int value);
    void updateProgress(double value);
    static int getPriority(MessageType mt);
    Overlay* getOverlay();
private:
    int priority = 0;
    MessageType messageType;
    Overlay* overlay = nullptr;
    bool overlayInitialized = false;

    void createOverlay();
};

#endif
