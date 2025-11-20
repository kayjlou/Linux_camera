/*
* Copyright 2016-2025
* This file is copyrighted by onexip GmbH.
* All rights reserved.
*
* Date:   2025-08-18
* Author: Thorsten Knoelker
*/

#ifndef LED_CONTROL_H
#define LED_CONTROL_H

#include "I2cCommunication.h"
#include <string>
#include <atomic>
#include "Constants.h"

class LedControl {
public:
    void setMax();
    void setReduced();
    void switchOff();
    int increase();
    int decrease();
    void applyBackupValue();
protected:
    void setValue(int value);
private:
    std::atomic<int> currentValue{Constants::DefaultLedValue};
    std::atomic<int> valueBackup{Constants::DefaultLedValue};

    static std::string intToHexString(int value);
    int convertToPercent(int value);
};

#endif
