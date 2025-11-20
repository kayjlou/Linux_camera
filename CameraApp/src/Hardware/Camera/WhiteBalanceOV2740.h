/*
* Copyright 2016-2025
* This file is copyrighted by onexip GmbH.
* All rights reserved.
*
* Date:   2025-05-05
* Author: Thorsten Knoelker
*/

#ifndef WHITE_BALANCE_H
#define WHITE_BALANCE_H

#include "CameraRegisterAccess.h"
#include <thread>
#include <atomic>
#include <chrono>
#include <vector>

class WhiteBalanceOV2740 {
public:
    enum class Status {
        Ready,
        Running,
        Finished
    };

    static constexpr double awbDuration = 5000.0;

    WhiteBalanceOV2740();
    void start();
    void reset();
    void applyManualGains();
    Status getStatus() const;
private:
    static constexpr std::chrono::milliseconds gainMeasureInterval = std::chrono::milliseconds(100);

    int redGain = 0;
    int greenGain = 0;
    int blueGain = 0;

    std::atomic<Status> status;
    std::thread awbThread;

    static bool setAwb(bool active);
    static int readRedGain(int *value);
    static int readGreenGain(int *value);
    static int readBlueGain(int *value);
    void collectGains();
};

#endif