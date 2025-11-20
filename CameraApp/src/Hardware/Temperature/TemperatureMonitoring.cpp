/*
* Copyright 2016-2025
* This file is copyrighted by onexip GmbH.
* All rights reserved.
*
* Date:   2025-08-18
* Author: Thorsten Knoelker
*/

#include "TemperatureMonitoring.h"
#include "MessageHandler.h"
#include "ActionHandler.h"
#include <iomanip>
#include <sstream> 
#include "Constants.h"

TemperatureMonitoring::~TemperatureMonitoring() {
    stopThread();
}

float TemperatureMonitoring::readTemperature() {
    std::string rawValueStr = I2cCommunication::read(I2cCommunication::Command::GetTemperature);

    if (rawValueStr.empty()) {
        printf("Error reading temperature\n");
        return -1;
    }

    int rawValue;

    try {
        rawValue = std::stoi(rawValueStr, nullptr, 16); // Convert hex string to integer
    } catch (const std::exception& e) {
        printf("Error converting temperature from string to integer");
        return -1;
    }

    return convertToCelsius(rawValue);
}

float TemperatureMonitoring::convertToCelsius(int rawValue)
{
    return static_cast<float>(rawValue) / 333.87f + 21.0f;
}

void TemperatureMonitoring::startThread() {
    // TODO: Deactivated temporarily
    return;

    keepRunning = true;
    monitoringThread = std::thread(&TemperatureMonitoring::monitorTemperature, this);
}

void TemperatureMonitoring::stopThread() {
    // TODO: Deactivated temporarily
    return;
    
    keepRunning = false;

    if (monitoringThread.joinable()) {
        monitoringThread.join();
    }
}

void TemperatureMonitoring::monitorTemperature() {
    while (keepRunning) {
        float t = readTemperature();
        #ifdef DEBUG
        temperature = t;
        #endif
        checkTemperature(t);
        std::this_thread::sleep_for(Constants::ReadTemperatureInterval);
    }
}

void TemperatureMonitoring::checkTemperature(float temperature) {
    if (temperature < 0) {
        //TODO: What to do in error case? Switch off LED?
    }

    if (temperature > Constants::CriticalTemperature) {
        if (temperatureState != TemperatureState::Critical) {
            temperatureState = TemperatureState::Critical;
            ActionHandler::getInstance().addAction(Action::ActionType::SwitchOffLed);
            MessageHandler::getInstance().set(Message::MessageType::TemperatureCritical);
        }
    } else if (temperature > Constants::WarningTemperature) {
        if (temperatureState != TemperatureState::High) {
            temperatureState = TemperatureState::High;
            ActionHandler::getInstance().addAction(Action::ActionType::SetReducedLedBrightness);
            MessageHandler::getInstance().set(Message::MessageType::TemperatureWarning);
        }
    }
}

#ifdef DEBUG
std::string TemperatureMonitoring::getTemperatureText() const {
    if (temperature < 0) {
        return "Temperature: error";
    } else {
        std::ostringstream oss;
        oss << std::fixed << std::setprecision(2) << temperature;
        return "Temperature: " + oss.str() + " °C";
    }
}
#endif
