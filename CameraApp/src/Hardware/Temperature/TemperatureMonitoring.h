/*
* Copyright 2016-2025
* This file is copyrighted by onexip GmbH.
* All rights reserved.
*
* Date:   2025-08-18
* Author: Thorsten Knoelker
*/

#ifndef TEMPERATURE_MONITORING_H
#define TEMPERATURE_MONITORING_H

#include "I2cCommunication.h"
#include <thread>
#include <atomic>
#include <chrono>
#include <string>

class TemperatureMonitoring {
public:
    ~TemperatureMonitoring();
    void startThread();

    #ifdef DEBUG
    std::string getTemperatureText() const;
    #endif
private:
    enum TemperatureState {
        Normal,
        High,
        Critical
    };

    std::atomic<bool> keepRunning{false};
    std::thread monitoringThread;
    TemperatureState temperatureState = Normal;

    static float readTemperature();
    static float convertToCelsius(int rawValue);
    void monitorTemperature();
    void checkTemperature(float temperature);
    void stopThread();

    #ifdef DEBUG
    std::atomic<float> temperature;
    #endif
};

#endif
