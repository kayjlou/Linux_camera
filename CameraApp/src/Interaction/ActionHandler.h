/*
* Copyright 2016-2025
* This file is copyrighted by onexip GmbH.
* All rights reserved.
*
* Date:   2025-05-05
* Author: Thorsten Knoelker
*/

#ifndef ACTION_HANDLER_H
#define ACTION_HANDLER_H

#include <deque>
#include "ControlBoard.h"
#include "Video.h"
#include "Action.h"
#include <optional>
#include <mutex>
#include <thread>
#include <atomic>
#include <chrono>

class ActionHandler {
public:
    ActionHandler(const ActionHandler&) = delete;
    ActionHandler& operator=(const ActionHandler&) = delete;
    static ActionHandler& getInstance();
    
    void addAction(const Action& action);
    bool recordingActive() const;
    void setControlBoard(ControlBoard* controlBoard);
    void setVideo(Video* video);
    std::optional<Action> getHandledAction();
    void startThread();
    void stopThread();
private:
    static constexpr std::chrono::milliseconds executionSleepTime = std::chrono::milliseconds(50);

    std::deque<Action> actionQueue;
    std::mutex actionQueueMutex;
    std::deque<Action> handledActionQueue;
    std::mutex handledActionQueueMutex;
    ControlBoard* controlBoard;
    Video* video;
    std::atomic<bool> keepRunning;
    std::thread executionThread;
    std::atomic<Action::ActionType> executingActionType;
    
    ActionHandler();
    ~ActionHandler();
    void addHandledAction(const Action& action);
    std::optional<Action> getAction();
    bool checkAlreadyQueued(const Action& action);
    void execute();
    void addFollowUpAction(const Action& currentAction);
    bool isActionExecutable(const Action& action);
};

#endif
