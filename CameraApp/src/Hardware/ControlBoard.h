/*
* Copyright 2016-2025
* This file is copyrighted by onexip GmbH.
* All rights reserved.
*
* Date:   2025-10-16
* Author: Thorsten Knoelker
*/

#ifndef CONTROL_BOARD_H
#define CONTROL_BOARD_H

#include "GmslConnection.h"
#include "Endoscope.h"
#include "UartCommunication.h"
#include "TemperatureMonitoring.h"
#include "ButtonHandler.h"

class ControlBoard {
public:
    ~ControlBoard();
    void initGmslConnection();
    bool checkGmslConnection(bool force = false);
    void resetCamera();
    void disconnectCamera();
    Endoscope* getEndoscope(int id);
    Endoscope* getActiveEndoscope() const;
    int getActiveEndoscopeId() const;
    int switchCamera();
    bool isGmslConnectionActive() const;
    void setAvailability(int endoscopeId, bool available);
    UartCommunication* getUartCommunication();
    void initCommunication(ButtonHandler* buttonHandler);
    TemperatureMonitoring* getTemperatureMonitoring();
    bool isAvailabilityInitialized() const;
private:
    static constexpr int checkInterval = 5;
    static constexpr int showConnectMessageThresholdInMs = 3000;

    GmslConnection gmslConnection;
    Endoscope endoscope1;
    Endoscope endoscope2;
    Endoscope* activeEndoscope = nullptr;
    int activeEndoscopeId = 0;
    std::chrono::time_point<std::chrono::high_resolution_clock> disconnectTime;
    int connectionCheckIteration = 0;
    bool gmslConnectionActive = false;
    UartCommunication uartCommunication;
    TemperatureMonitoring temperatureMonitoring;

    void applyCameraSettings();
    void updateGmslConnection(int endoscopeId, bool available);
    void updateActiveEndoscope();
};

#endif
