/*
* Copyright 2016-2025
* This file is copyrighted by onexip GmbH.
* All rights reserved.
*
* Date:   2025-05-05
* Author: Thorsten Knoelker
*/

#ifndef ACTION_H
#define ACTION_H

#include "ControlBoard.h"
#include "Video.h"

class Action {
public:
    enum class ActionType {
        None,
        WhiteBalance,
        IncreaseBrightness,
        DecreaseBrightness,
        IncreaseLedBrightness,
        DecreaseLedBrightness,
        SetReducedLedBrightness,
        SwitchOffLed,
        Snapshot,
        PrepareVideoRecording,
        StartVideoRecording,
        StopVideoRecording,
        SavingVideoRecording,
        PrepareCameraSwitching,
        SwitchCamera,
        CameraAvailability,

        #ifdef DEBUG
        ReadImxTemperature,
        ResetCamera,
        ShowVersions,
        IntegrationTestModeToggle,
        DisplayTemperatureMonitoringToggle,
        SimulateDistalLimitSwitch,
        SimulateUartCommunicationInterruption,
        ToggleVideoFreezeSimulation,
        #endif
    };

    Action(ActionType type);
    Action(ActionType type, int cameraId);
    Action(ActionType type, int cameraId, bool active);
    void execute(ControlBoard* controlBoard, Video* video);
    ActionType getActionType() const;
    int getResultInt() const;
    std::string getResultString() const;
    int getCameraId() const;
    bool getActive() const;
private:
    ActionType type;
    int resultInt;
    std::string resultString;
    int cameraId;
    bool active;
};

#endif
