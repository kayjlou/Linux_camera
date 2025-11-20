/*
* Copyright 2016-2025
* This file is copyrighted by onexip GmbH.
* All rights reserved.
*
* Date:   2025-10-16
* Author: Thorsten Knoelker
*/

#include "ControlBoard.h"
#include "MessageHandler.h"
#include "Utils.h"

ControlBoard::~ControlBoard() {
    #ifndef SIMULATE_ALL_HARDWARE
    gmslConnection.stopThread();
    #endif
}

void ControlBoard::initGmslConnection() {
    #ifdef SIMULATE_ALL_HARDWARE
    activeEndoscope = &endoscope1;
    gmslConnectionActive = true;
    return;
    #endif

    if (endoscope1.isAvailable()) {
        gmslConnection.setSelectedCamera(1);
    } else if (endoscope2.isAvailable()) {
        gmslConnection.setSelectedCamera(2);
    } else {
        gmslConnection.setSelectedCamera(0);
        gmslConnection.pause();
    }

    disconnectTime = std::chrono::high_resolution_clock::now();
    gmslConnection.startThread();
}

bool ControlBoard::checkGmslConnection(bool force) {
    #ifdef SIMULATE_ALL_HARDWARE
        return true;
    #else
        if (force || connectionCheckIteration < checkInterval) {
            bool wasConnected = gmslConnectionActive;
            gmslConnectionActive = gmslConnection.isConnected();

            if (gmslConnectionActive && !wasConnected) {
                updateActiveEndoscope();
                printf("Camera connected again: applyAllSettings\n");
                // Camera is connected again after disconnect, therefore settings need to be applied again
                applyCameraSettings();

                if (Utils::timePassedSince(disconnectTime) > showConnectMessageThresholdInMs) {
                    // Only show disconnected message if camera was disconnected for longer
                    MessageHandler::getInstance().set(Message::MessageType::CameraConnected);
                } else {
                    // If camera was disconnected only for a brief moment, just remove the disconnect message
                    MessageHandler::getInstance().remove(Message::MessageType::CameraDisconnected);
                }
            } else if (!gmslConnectionActive && wasConnected) {
                MessageHandler::getInstance().set(Message::MessageType::CameraDisconnected);
                disconnectTime = std::chrono::high_resolution_clock::now();
            }
        }
        
        return gmslConnectionActive;
    #endif
}

bool ControlBoard::isGmslConnectionActive() const {
    return gmslConnectionActive;
}

void ControlBoard::resetCamera() {
    printf("Reset camera\n");
    gmslConnection.resetCamera();
    activeEndoscope->getCamera()->applyAllSettings();
}

void ControlBoard::disconnectCamera() {
    gmslConnection.disconnect();
    gmslConnectionActive = false;
    updateActiveEndoscope();    
}

Endoscope* ControlBoard::getEndoscope(int id) {
    if (id == 1) {
        return &endoscope1;
    } else if (id == 2) {
        return &endoscope2;
    }

    return nullptr;
}

Endoscope* ControlBoard::getActiveEndoscope() const {
    return activeEndoscope;
}

int ControlBoard::getActiveEndoscopeId() const {
    return activeEndoscopeId;
}

int ControlBoard::switchCamera() {
    int cameraIndexOld = activeEndoscopeId;
    bool available1 = endoscope1.isAvailable();
    bool available2 = endoscope2.isAvailable();

    if (activeEndoscopeId == 0 && !available1 && !available2) {
        printf("Switch camera not possible, no camera available\n");
        return 0;
    }

    if ((cameraIndexOld == 1 && !available2)
        || (cameraIndexOld == 2 && !available1)) {
        printf("Switch camera not possible, other camera not available\n");
        return cameraIndexOld;
    }

    int cameraIndexNew;

    if (cameraIndexOld == 1) {
        cameraIndexNew = 2;
    } else if (cameraIndexOld == 2) {
        cameraIndexNew = 1;
    } else {
        cameraIndexNew = available1 ? 1 : 2;
    }

    gmslConnection.switchCamera(cameraIndexNew);
    printf("Switched camera from %d to %d\n", cameraIndexOld, cameraIndexNew);

    activeEndoscopeId = cameraIndexNew;

    if (activeEndoscopeId == 1) {
        activeEndoscope = &endoscope1;
        endoscope2.getLedControl()->setReduced();
        endoscope1.getLedControl()->applyBackupValue();
    } else if (activeEndoscopeId == 2) {
        activeEndoscope = &endoscope2;
        endoscope1.getLedControl()->setReduced();
        endoscope2.getLedControl()->applyBackupValue();
    }

    applyCameraSettings();
    uartCommunication.updateCameraState(cameraIndexNew, true);

    return cameraIndexNew;
}

void ControlBoard::applyCameraSettings() {
    if (activeEndoscope) {
        activeEndoscope->getCamera()->applyAllSettings();
    }
}

void ControlBoard::setAvailability(int endoscopeId, bool available) {
    printf("Set endoscope %d available: %d\n", endoscopeId, available);

    if (endoscopeId == 1) {
        endoscope1.setAvailable(available);
    } else if (endoscopeId == 2) {
        endoscope2.setAvailable(available);  
    }

    updateGmslConnection(endoscopeId, available);
}

void ControlBoard::updateGmslConnection(int endoscopeId, bool available) {
    if (activeEndoscopeId == endoscopeId) {
        if (!available) {
            gmslConnection.pause();
        }
    }

    if (available && !gmslConnection.isConnected()) {
        gmslConnection.setSelectedCamera(endoscopeId);
        gmslConnection.resume();
    }
}

void ControlBoard::updateActiveEndoscope() {
    if (gmslConnectionActive) {
        if (gmslConnection.getSelectedCamera() == 1) {
            activeEndoscope = &endoscope1;
            activeEndoscopeId = 1;
        } else if (gmslConnection.getSelectedCamera() == 2) {
            activeEndoscope = &endoscope2;
            activeEndoscopeId = 2;
        }
    } else {
        activeEndoscope = nullptr;
        activeEndoscopeId = 0;
    }
}

UartCommunication* ControlBoard::getUartCommunication() {
    return &uartCommunication;
}

void ControlBoard::initCommunication(ButtonHandler* buttonHandler) {
    temperatureMonitoring.startThread();
    uartCommunication.init(buttonHandler);
}

TemperatureMonitoring* ControlBoard::getTemperatureMonitoring() {
    return &temperatureMonitoring;
}

bool ControlBoard::isAvailabilityInitialized() const {
    return endoscope1.isAvailabilityInitialized() && endoscope2.isAvailabilityInitialized();
}
