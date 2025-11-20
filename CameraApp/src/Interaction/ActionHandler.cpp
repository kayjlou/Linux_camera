/*
* Copyright 2016-2025
* This file is copyrighted by onexip GmbH.
* All rights reserved.
*
* Date:   2025-05-05
* Author: Thorsten Knoelker
*/

#include "ActionHandler.h"
#include "Constants.h"

ActionHandler::ActionHandler() {
    startThread();
}

ActionHandler::~ActionHandler() {
    stopThread();
}

ActionHandler& ActionHandler::getInstance() {
    static ActionHandler instance;
    return instance;
}

void ActionHandler::addAction(const Action& action) {
    if (!isActionExecutable(action)) {
        return;
    }

    if (checkAlreadyQueued(action)) {
        return;
    }

    std::lock_guard<std::mutex> lock(actionQueueMutex);
    actionQueue.push_back(action);
}

std::optional<Action> ActionHandler::getAction() {
    if (actionQueue.empty()) {
        return std::nullopt;
    }

    std::lock_guard<std::mutex> lock(actionQueueMutex);
    Action action = actionQueue.front();
    actionQueue.pop_front();

    return action;
}

bool ActionHandler::checkAlreadyQueued(const Action& action) {
    if (executingActionType == action.getActionType()) {
        return true;
    }

    for (Action& a: actionQueue) {
        if (a.getActionType() == action.getActionType()) {
            if (action.getActionType() == Action::ActionType::IncreaseBrightness
                && action.getActionType() == Action::ActionType::DecreaseBrightness) {
                    // For brightness, replace existing action by the new action
                    std::lock_guard<std::mutex> lock(actionQueueMutex);
                    a = action;
                }

            return true;
        }
    }

    return false;
}

void ActionHandler::execute() {
    while (keepRunning) {
        auto actionOptional = getAction();

        if (actionOptional.has_value()) {
            auto action = actionOptional.value();
            auto actionType = action.getActionType();
            executingActionType = actionType;

            if (isActionExecutable(action)) {
                action.execute(controlBoard, video);
                addHandledAction(action);
                addFollowUpAction(action);
            }

            executingActionType = Action::ActionType::None;
        }
        
        std::this_thread::sleep_for(executionSleepTime);
    }
}

bool ActionHandler::isActionExecutable(const Action& action) {
    #ifdef SIMULATE_ALL_HARDWARE
        return true;
    #else

    auto actionType = action.getActionType();

    if (actionType == Action::ActionType::SwitchCamera
        || actionType == Action::ActionType::PrepareCameraSwitching
        || actionType == Action::ActionType::CameraAvailability
        || actionType == Action::ActionType::ShowVersions
        || actionType == Action::ActionType::IntegrationTestModeToggle
        || actionType == Action::ActionType::SimulateDistalLimitSwitch
        ) {
        return true;
    }

    // For all other actions a camera connection is required
    return controlBoard->isGmslConnectionActive() && controlBoard->getActiveEndoscope() != nullptr;
    #endif
}

void ActionHandler::addFollowUpAction(const Action& currentAction) {
    auto actionType = currentAction.getActionType();

    if (actionType == Action::ActionType::PrepareVideoRecording && currentAction.getResultInt() > 0) {
        std::this_thread::sleep_for(std::chrono::milliseconds(Constants::RecordingDelayTimeInMilliseconds) - executionSleepTime);
        addAction(Action::ActionType::StartVideoRecording);
    } else if (actionType == Action::ActionType::StopVideoRecording) {
        addAction(Action::ActionType::SavingVideoRecording);
    } else if (actionType == Action::ActionType::PrepareCameraSwitching) {
        addAction(Action::ActionType::SwitchCamera);
    }
}

bool ActionHandler::recordingActive() const {
    return video->isRecording();
}

void ActionHandler::setControlBoard(ControlBoard* controlBoard) {
    this->controlBoard = controlBoard;
}

void ActionHandler::setVideo(Video* video) {
    this->video = video;
}

void ActionHandler::addHandledAction(const Action& action) {
    std::lock_guard<std::mutex> lock(handledActionQueueMutex);
    handledActionQueue.push_back(action);
}

std::optional<Action> ActionHandler::getHandledAction() {
    if (handledActionQueue.empty()) {
        return std::nullopt;
    }

    std::lock_guard<std::mutex> lock(handledActionQueueMutex);
    Action action = handledActionQueue.front();
    handledActionQueue.pop_front();

    return action;
}

void ActionHandler::startThread() {
    keepRunning = true;
    executionThread = std::thread(&ActionHandler::execute, this);
}

void ActionHandler::stopThread() {
    keepRunning = false;

    if (executionThread.joinable()) {
        executionThread.join();
    }
}
