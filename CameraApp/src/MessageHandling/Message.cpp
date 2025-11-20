/*
* Copyright 2016-2025
* This file is copyrighted by onexip GmbH.
* All rights reserved.
*
* Date:   2025-05-05
* Author: Thorsten Knoelker
*/

#include "Message.h"
#include "MessageStrings.h"
#include "WhiteBalanceOverlay.h"
#include "BrightnessOverlay.h"

Message::Message(MessageType messageType) {
    this->messageType = messageType;
    priority = getPriority(messageType);
    createOverlay();
}

Message::~Message() {
    if (overlay) {
        delete overlay;
    }
}

int Message::getPriority(MessageType mt) {
    switch (mt) {
        case MessageType::CameraConnected:
        return 100;
        case MessageType::CameraDisconnected:
        return 100;
        case MessageType::FrozenFrame:
        return 90;
        case MessageType::NoUsbStorage:
        return 70;
        case MessageType::WhiteBalance:
        return 60;
        case MessageType::SavingVideo:
        return 50;
        default:
        return 0;
    }
}

Message::MessageType Message::getType() const {
    return messageType;
}

int Message::getPriority() const {
    return priority;
}

bool Message::isExpired() const {
    return overlay ? overlay->isExpired() : false;
}

void Message::updateValue(int value) {
    if (!overlay) {
        return;
    }

    if (messageType == MessageType::Gain
        || messageType == MessageType::LedBrightness) {
        BrightnessOverlay* brightnessOverlay = dynamic_cast<BrightnessOverlay*>(overlay);
        brightnessOverlay->setBrightness(value);
    }
}

void Message::updateProgress(double value) {
    if (!overlay) {
        return;
    }

    if (messageType == MessageType::WhiteBalance) {
        WhiteBalanceOverlay* whiteBalanceOverlay = dynamic_cast<WhiteBalanceOverlay*>(overlay);
        whiteBalanceOverlay->updateProgress(value);
    }
}

void Message::createOverlay() {
    std::string header;
    std::string text;

    switch (messageType) {
        case MessageType::CameraConnectedInitially:
        header = MessageStrings::CameraConnectedHeader;
        text = MessageStrings::CameraConnectedText;
        overlay = new MessageOverlay(MessageOverlay::MessageType::success, Icon::camera, true, header, text);
        break;
        case MessageType::CameraConnected:
        header = MessageStrings::CameraConnectedHeader;
        text = MessageStrings::CameraConnectedText;
        overlay = new MessageOverlay(MessageOverlay::MessageType::success, Icon::camera, true, header, text, Constants::CameraConnectedAppearanceTimeInMilliseconds);
        break;
        case MessageType::CameraDisconnected:
        header = MessageStrings::CameraDisconnectedHeader;
        text = MessageStrings::CameraDisconnectedText;
        overlay = new MessageOverlay(MessageOverlay::MessageType::error, Icon::disconnected, false, header, text);
        break;
        case MessageType::FrozenFrame:
        header = MessageStrings::FrozenFrameHeader;
        text = MessageStrings::FrozenFrameText;
        overlay = new MessageOverlay(MessageOverlay::MessageType::error, Icon::camera, false, header, text);
        break;
        case MessageType::NoUsbStorage:
        header = MessageStrings::NoUsbStorageHeader;
        text = MessageStrings::NoUsbStorageText;
        overlay = new MessageOverlay(MessageOverlay::MessageType::error, Icon::warning, true, header, text, Constants::NoUsbStorageAppearanceTimeInMilliseconds);
        break;
        case MessageType::RecordingAboutToStart:
        header = MessageStrings::RecordingAboutToStartHeader;
        text = MessageStrings::RecordingAboutToStartText;
        overlay = new MessageOverlay(MessageOverlay::MessageType::info, Icon::warning, true, header, text, Constants::RecordingDelayTimeInMilliseconds);
        break;
        case MessageType::SavingVideo:
        header = MessageStrings::SavingVideoHeader;
        text = MessageStrings::SavingVideoText;
        overlay = new MessageOverlay(MessageOverlay::MessageType::info, Icon::warning, false, header, text);
        break;
        case MessageType::SavingVideoSuccessful:
        header = MessageStrings::SavingVideoSuccessfulHeader;
        text = MessageStrings::SavingVideoSuccessfulText;
        overlay = new MessageOverlay(MessageOverlay::MessageType::success, Icon::check, true, header, text, Constants::SavingVideoSuccessfulAppearanceTimeInMilliseconds);
        break;
        case MessageType::SavingVideoFailed:
        header = MessageStrings::SavingVideoFailedHeader;
        text = MessageStrings::SavingVideoFailedText;
        overlay = new MessageOverlay(MessageOverlay::MessageType::error, Icon::warning, true, header, text, Constants::SavingVideoFailedAppearanceTimeInMilliseconds);
        break;
        case MessageType::WhiteBalance:
        overlay = new WhiteBalanceOverlay();
        break;
        case MessageType::Gain:
        overlay = new BrightnessOverlay("Gain");
        break;
        case MessageType::LedBrightness:
        overlay = new BrightnessOverlay();
        break;
        case MessageType::TemperatureWarning:
        header = MessageStrings::TemperatureWarningHeader;
        text = MessageStrings::TemperatureWarningText;
        overlay = new MessageOverlay(MessageOverlay::MessageType::error, Icon::warning, true, header, text);
        break;
        case MessageType::TemperatureCritical:
        header = MessageStrings::TemperatureCriticalHeader;
        text = MessageStrings::TemperatureCriticalText;
        overlay = new MessageOverlay(MessageOverlay::MessageType::error, Icon::warning, true, header, text);
        break;
        case MessageType::UartTimeout:
        header = MessageStrings::UartTimeoutHeader;
        text = MessageStrings::UartTimeoutText;
        overlay = new MessageOverlay(MessageOverlay::MessageType::error, Icon::warning, true, header, text, 3000);
        break;
        default:
        break;
    }
}

Overlay* Message::getOverlay() {
    if (overlay && !overlayInitialized) {
        overlay->init();
        overlayInitialized = true;
    }

    return overlay;
}
