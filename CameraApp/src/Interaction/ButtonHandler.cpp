/*
* Copyright 2016-2025
* This file is copyrighted by onexip GmbH.
* All rights reserved.
*
* Date:   2025-05-05
* Author: Thorsten Knoelker
*/

#include "ButtonHandler.h"
#include "Utils.h"
#include "ActionHandler.h"

void ButtonHandler::press(Button button) {
    if (button == Button::None) {
        return;
    }

    std::lock_guard<std::mutex> lock(evaluationLock);
    pressStartTime = std::chrono::high_resolution_clock::now();
    longPressHandled = false;
    currentButton = button;
}

void ButtonHandler::release(Button button) {
    std::lock_guard<std::mutex> lock(evaluationLock);

    if (button == Button::None || button != currentButton) {
        return;
    }

    if (longPressHandled) {
        longPressHandled = false;
        return;
    }

    handle(currentButton, false);
    currentButton = Button::None;
}

void ButtonHandler::handle(uint8_t buttonId, uint8_t pressType) {
    Button button = uint8ToButton(buttonId);

    if (pressType == 0) {
        #ifdef DEBUG
        printf("Button released: %d\n", buttonId);
        #endif
        release(button);
    } else {
        #ifdef DEBUG
        printf("Button pressed: %d\n", buttonId);
        #endif
        press(button);
    }
}

void ButtonHandler::handle(Button button, bool longPress) {
    if (longPress) {
        longPressHandled = true;
        currentButton = Button::None;
    }
    
    switch (button)
    {
    case Button::Top:
        if (longPress) {
            ledAdjustMode = true;
        }

        if (ledAdjustMode) {
            ActionHandler::getInstance().addAction(Action::ActionType::IncreaseLedBrightness);
            lastLedAdjustmentTime = std::chrono::high_resolution_clock::now();
        } else {
            ActionHandler::getInstance().addAction(Action::ActionType::IncreaseBrightness);
        }
        break;
    case Button::Bottom:
        if (longPress) {
            ledAdjustMode = true;
        }

        if (ledAdjustMode) {
            ActionHandler::getInstance().addAction(Action::ActionType::DecreaseLedBrightness);
            lastLedAdjustmentTime = std::chrono::high_resolution_clock::now();
        } else {
            ActionHandler::getInstance().addAction(Action::ActionType::DecreaseBrightness);
        }
        break;
    case Button::Left:
        if (longPress) {
            ActionHandler::getInstance().addAction(Action::ActionType::WhiteBalance);
        } else {
            ActionHandler::getInstance().addAction(Action::ActionType::Snapshot);
        }
        break;
    case Button::Right:
        if (ActionHandler::getInstance().recordingActive()) {
            ActionHandler::getInstance().addAction(Action::ActionType::StopVideoRecording);
        } else {
            if (longPress) {
                ActionHandler::getInstance().addAction(Action::ActionType::PrepareCameraSwitching);
            } else {
                ActionHandler::getInstance().addAction(Action::ActionType::PrepareVideoRecording);
            }
        }
        break;
    default:
        break;
    }
}

bool ButtonHandler::longPressReached(Button button) const {
    auto currentTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - pressStartTime);
    auto elapsedTime = duration.count();

    switch (button)
    {
    case Button::Top:
        return elapsedTime > gainLongPress;
    case Button::Bottom:
        return elapsedTime > gainLongPress;
    case Button::Left:
        return elapsedTime > whiteBalanceLongPress;
    case Button::Right:
        return elapsedTime > cameraSwitchLongPress;
    default:
        return false;
    }
}

void ButtonHandler::evaluate() {
    std::lock_guard<std::mutex> lock(evaluationLock);
    evaluateGainAdjustmentMode();
    bool longPress = longPressReached(currentButton);

    if (longPress) {
        handle(currentButton, longPress);
    }
}

Button ButtonHandler::uint8ToButton(uint8_t id) {
    switch (id) {
        case 0:
            return Button::Top;
        case 1:
            return Button::Right;
        case 2:
            return Button::Bottom;
        case 3:
            return Button::Left;
        default:
            return Button::None;
    }
}

void ButtonHandler::evaluateGainAdjustmentMode() {
    if (!ledAdjustMode) {
        return;
    }
    
    if (Utils::timePassedSince(lastLedAdjustmentTime) > 5000) {
        ledAdjustMode = false;
    }
}
