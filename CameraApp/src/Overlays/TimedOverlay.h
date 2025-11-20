/*
* Copyright 2016-2025
* This file is copyrighted by onexip GmbH.
* All rights reserved.
*
* Date:   2025-05-05
* Author: Thorsten Knoelker
*/

#ifndef TIMED_OVERLAY_H
#define TIMED_OVERLAY_H

#include "Overlay.h"
#include "Constants.h"
#include <chrono>

class TimedOverlay : public Overlay {
public:
    TimedOverlay();
    TimedOverlay(bool timerEnabled);
    TimedOverlay(bool timerEnabled, int appearanceTimeInMilliseconds);
protected:
    bool evaluateTimer();
    bool getTimerEnabled() const;
    void setTimerEnabled(bool enabled);
    void setExpirationCheckFrequency(int frequency);
    int64_t getElapsedTime() const;
    void resetTimer();
    double getProgress() const;
    bool isExpired() const override;
private:
    bool enabled = true; // If false, timer is disabled and the overlay won't expire
    bool timerStarted = false;
    std::chrono::time_point<std::chrono::high_resolution_clock> startTime;
    bool expired = false;
    int expirationCheckIteration = 0;
    int64_t elapsedTime = 0;
    int appearanceTimeInMilliseconds = Constants::MessageAppearanceTimeInMilliseconds;

    // Defines how many checkExpiration calls are skipped until the calculations
    // to check whether timer has expired are performed again.
    int expirationCheckFrequency = 2;

    void startTimer();
    void checkExpiration();
};

#endif
