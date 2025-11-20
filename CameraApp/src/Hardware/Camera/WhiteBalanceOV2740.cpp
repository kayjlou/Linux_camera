/*
* Copyright 2016-2025
* This file is copyrighted by onexip GmbH.
* All rights reserved.
*
* Date:   2025-05-05
* Author: Thorsten Knoelker
*/

#include "WhiteBalanceOV2740.h"
#include "Utils.h"

WhiteBalanceOV2740::WhiteBalanceOV2740() {
    status = Status::Ready;
}

void WhiteBalanceOV2740::start() {
    if (status != Status::Ready) {
        return;
    }

    status = Status::Running;
    awbThread = std::thread(&WhiteBalanceOV2740::collectGains, this);
}

void WhiteBalanceOV2740::reset() {
    if (awbThread.joinable()) {
        awbThread.join();
    }

    status = Status::Ready;
}

bool WhiteBalanceOV2740::setAwb(bool active) {
    u8 value;
    int ret = CameraRegisterAccess::getInstance().readRegister(0x5000, &value);

    if (ret == RET_FAIL) {
        return false;
    }

    if (active) {

        CameraRegisterAccess::getInstance().write16bitValue(0x500A, 0x500B, 1024);
        CameraRegisterAccess::getInstance().write16bitValue(0x500C, 0x500D, 1024);
        CameraRegisterAccess::getInstance().write16bitValue(0x500E, 0x500F, 1024);
        value &= ~0x10; //AWB active means manual AWB gain off
        CameraRegisterAccess::getInstance().writeRegister(0x5202, 0);
    } else {
        value |= 0x10;
        CameraRegisterAccess::getInstance().writeRegister(0x5202, 1);
    }

    ret = CameraRegisterAccess::getInstance().writeRegister(0x5000, value);

    return ret == RET_OK;
}

int WhiteBalanceOV2740::readRedGain(int *value) {
    //return CameraRegisterAccess::getInstance().read16BitValue(0x5213, 0x5214, value);
    return CameraRegisterAccess::getInstance().read16BitValue(0x520D, 0x520E, value);
}

int WhiteBalanceOV2740::readGreenGain(int *value) {
    //return CameraRegisterAccess::getInstance().read16BitValue(0x5211, 0x5212, value);
    return CameraRegisterAccess::getInstance().read16BitValue(0x520B, 0x520C, value);
}

int WhiteBalanceOV2740::readBlueGain(int *value) {
    //return CameraRegisterAccess::getInstance().read16BitValue(0x520F, 0x5210, value);
    return CameraRegisterAccess::getInstance().read16BitValue(0x5209, 0x520A, value);
}

void WhiteBalanceOV2740::applyManualGains() {
    if (redGain == 0 || greenGain == 0 || blueGain == 0) {
        return;
    }

    setAwb(false);
    int r = (redGain - 1024)*0.25 + 1024; // Manual tweaking of RGB gains.
    int g = greenGain;
    int b = (blueGain - 1024)*1.25 + 1024;
    CameraRegisterAccess::getInstance().write16bitValue(0x500A, 0x500B, r);
    CameraRegisterAccess::getInstance().write16bitValue(0x500C, 0x500D, g);
    CameraRegisterAccess::getInstance().write16bitValue(0x500E, 0x500F, b);
    /*CameraRegisterAccess::getInstance().write16bitValue(0x5207, 0x5208, redGain);
    CameraRegisterAccess::getInstance().write16bitValue(0x5205, 0x5206, greenGain);
    CameraRegisterAccess::getInstance().write16bitValue(0x5203, 0x5204, blueGain);*/
}

void WhiteBalanceOV2740::collectGains() {
    std::vector<int> collectedRedGains;
    std::vector<int> collectedGreenGains;
    std::vector<int> collectedBlueGains;

    auto start = std::chrono::steady_clock::now();
    auto end = start + std::chrono::duration<double, std::milli>(awbDuration);
    setAwb(true);

    while (std::chrono::steady_clock::now() < end) {
        std::this_thread::sleep_for(gainMeasureInterval);

        int value,ret;
        ret = readRedGain(&value);

        if (ret == RET_OK) {
            collectedRedGains.push_back(value);
        }

        ret = readGreenGain(&value);

        if (ret == RET_OK) {
            collectedGreenGains.push_back(value);
        }

        ret = readBlueGain(&value);

        if (ret == RET_OK) {
            collectedBlueGains.push_back(value);
        }
    }

    // Average gains for each color channel
    redGain = Utils::average(collectedRedGains);
    greenGain = Utils::average(collectedGreenGains);
    blueGain = Utils::average(collectedBlueGains);

    printf("redGain: %d\n", redGain);
    printf("greenGain: %d\n", greenGain);
    printf("blueGain: %d\n", blueGain);
    printf("--------\n");

    applyManualGains();
    status = Status::Finished;
}

WhiteBalanceOV2740::Status WhiteBalanceOV2740::getStatus() const {
    return status;
}
