/*
 * Copyright 2016-2025
 * This file is copyrighted by onexip GmbH.
 * All rights reserved.
 *
 * Date:   2025-05-05
 * Author: Thorsten Knoelker
 */

#include "Action.h"
#include "ImxTemperature.h"
#include "Versions.h"

#ifdef DEBUG
#include "IntegrationTesting.h"
#endif

Action::Action(ActionType type)
{
    this->type = type;
}

Action::Action(ActionType type, int cameraId)
{
    this->type = type;
    this->cameraId = cameraId;
}

Action::Action(ActionType type, int cameraId, bool active)
{
    this->type = type;
    this->cameraId = cameraId;
    this->active = active;
}

void Action::execute(ControlBoard* controlBoard, Video* video)
{
    switch (type)
    {
    case ActionType::WhiteBalance:
        video->startWhiteBalance();
        break;
    case ActionType::IncreaseBrightness:
        resultInt = controlBoard->getActiveEndoscope()->getCamera()->increaseBrightness();
        break;
    case ActionType::DecreaseBrightness:
        resultInt = controlBoard->getActiveEndoscope()->getCamera()->decreaseBrightness();
        break;
    case ActionType::IncreaseLedBrightness:
        #ifndef SIMULATE_ALL_HARDWARE
        resultInt = controlBoard->getActiveEndoscope()->getLedControl()->increase();
        #endif
        break;
    case ActionType::DecreaseLedBrightness:
        #ifndef SIMULATE_ALL_HARDWARE
        resultInt = controlBoard->getActiveEndoscope()->getLedControl()->decrease();
        #endif
        break;
    case ActionType::SetReducedLedBrightness:
        controlBoard->getActiveEndoscope()->getLedControl()->setReduced();
        break;
    case ActionType::SwitchOffLed:
        controlBoard->getActiveEndoscope()->getLedControl()->switchOff();
        break;
    case ActionType::Snapshot:
        resultInt = video->snapshot();
        break;
    case ActionType::PrepareVideoRecording:
        resultInt = video->prepareRecording();
        break;
    case ActionType::StartVideoRecording:
        resultInt = video->startRecording();
        break;
    case ActionType::StopVideoRecording:
        video->stopRecording();
        break;
    case ActionType::SavingVideoRecording:
        resultInt = video->finishRecording();
        break;
    case ActionType::SwitchCamera:
        printf("Action: SwitchCamera\n");
        resultInt = controlBoard->switchCamera();
        video->switchCamera(resultInt);
        break;
    case ActionType::CameraAvailability:
        controlBoard->setAvailability(cameraId, active);
        break;
#ifdef DEBUG
    case ActionType::ReadImxTemperature:
        resultString = ImxTemperature::getTemperatureString();
        break;
    case ActionType::ResetCamera:
        controlBoard->resetCamera();
        break;
    case ActionType::ShowVersions:
        resultString = Versions::getCompleteVersionString();
        break;
    case ActionType::IntegrationTestModeToggle:
        {
            bool current = IntegrationTesting::getInstance().isActive();
            IntegrationTesting::getInstance().setActive(!current);
            resultInt = !current;
        }
        break;
    case ActionType::DisplayTemperatureMonitoringToggle:
        {
            bool current = IntegrationTesting::getInstance().getTemperatureMonitoring();
            IntegrationTesting::getInstance().setTemperatureMonitoring(!current);
            resultInt = !current;
        }
        break;
    case ActionType::SimulateDistalLimitSwitch:
        {
            auto endoscope = controlBoard->getEndoscope(cameraId);
            active = false;

            if (endoscope)
            {
                active = !endoscope->isAvailable();
                controlBoard->getUartCommunication()->simulateDistalLimitSwitch(cameraId, active);
            }
        }
        break;
    case ActionType::SimulateUartCommunicationInterruption:
        controlBoard->getUartCommunication()->simulateCommunicationTimeout();
        break;
    case ActionType::ToggleVideoFreezeSimulation:
        video->toggleSimulateVideoFreeze();
        break;
#endif

    default:
        break;
    }
}

Action::ActionType Action::getActionType() const
{
    return type;
}

int Action::getResultInt() const
{
    return resultInt;
}

std::string Action::getResultString() const
{
    return resultString;
}

int Action::getCameraId() const
{
    return cameraId;
}

bool Action::getActive() const
{
    return active;
}
