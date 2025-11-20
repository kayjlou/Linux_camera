/*
* Copyright 2016-2025
* This file is copyrighted by onexip GmbH.
* All rights reserved.
*
* Date:   2025-05-05
* Author: Thorsten Knoelker
*/

#include "TimedOverlay.h"

TimedOverlay::TimedOverlay() {
}

TimedOverlay::TimedOverlay(bool timerEnabled) {
    enabled = timerEnabled;
}

TimedOverlay::TimedOverlay(bool timerEnabled, int appearanceTimeInMilliseconds) {
    enabled = timerEnabled;
    this->appearanceTimeInMilliseconds = appearanceTimeInMilliseconds;
}

bool TimedOverlay::evaluateTimer() {
    if (!enabled) {
        return true;
    }

    if (expired) {
        return false;
    }

    if (!timerStarted) {
        startTimer();
        return true;
    }

    checkExpiration();

    return !expired;
}

bool TimedOverlay::getTimerEnabled() const {
    return enabled;
}

void TimedOverlay::setTimerEnabled(bool enabled) {
    if (this->enabled != enabled) {
        if (enabled) {
            resetTimer();
            startTimer();
        }

        this->enabled = enabled;
    }
}

void TimedOverlay::startTimer() {
    startTime = std::chrono::high_resolution_clock::now();
    timerStarted = true;
    expired = false;
}

void TimedOverlay::checkExpiration() {
    if (expirationCheckIteration >= expirationCheckFrequency) {
        auto currentTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - startTime);
        elapsedTime = duration.count();

        if (elapsedTime > appearanceTimeInMilliseconds) {
            expired = true;
        }

        expirationCheckIteration = 0;
    } else {
        expirationCheckIteration++;
    }
}

void TimedOverlay::setExpirationCheckFrequency(int frequency) {
    expirationCheckFrequency = frequency;
}

int64_t TimedOverlay::getElapsedTime() const {
    return elapsedTime;
}

void TimedOverlay::resetTimer() {
    startTime = std::chrono::high_resolution_clock::now();
    expired = false;
    elapsedTime = 0;
}

double TimedOverlay::getProgress() const {
    double progress = elapsedTime / appearanceTimeInMilliseconds;

    if (progress < 0.0) {
        return 0.0;
    }

    if (progress > 1.0) {
        return 1.0;
    }

    return progress;
}

bool TimedOverlay::isExpired() const {
    return expired;
}
