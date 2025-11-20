/*
* Copyright 2016-2025
* This file is copyrighted by onexip GmbH.
* All rights reserved.
*
* Date:   2025-05-05
* Author: Thorsten Knoelker
*/

#ifndef BUTTON_HANDLER_H
#define BUTTON_HANDLER_H

#include <chrono>
#include <mutex>
#include <atomic>
#include "Button.h"

class ButtonHandler {
public:
    void press(Button button);
    void release(Button button);
    void evaluate();
    void handle(uint8_t buttonId, uint8_t pressType);
private:
    const int gainLongPress = 5000;
    const int whiteBalanceLongPress = 2000;
    const int cameraSwitchLongPress = 2000;    

    Button currentButton = Button::None;
    std::atomic<bool> longPressHandled{false};
    std::chrono::time_point<std::chrono::high_resolution_clock> pressStartTime;
    std::mutex evaluationLock;
    bool ledAdjustMode = false;
    std::chrono::time_point<std::chrono::high_resolution_clock> lastLedAdjustmentTime;

    bool longPressReached(Button button) const;
    void handle(Button button, bool longPress);
    static Button uint8ToButton(uint8_t id);
    void evaluateGainAdjustmentMode();
};

#endif
