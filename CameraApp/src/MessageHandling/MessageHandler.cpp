/*
* Copyright 2016-2025
* This file is copyrighted by onexip GmbH.
* All rights reserved.
*
* Date:   2025-05-05
* Author: Thorsten Knoelker
*/

#include "MessageHandler.h"

MessageHandler::MessageHandler() {}

MessageHandler::~MessageHandler() {}

MessageHandler& MessageHandler::getInstance() {
    static MessageHandler instance;
    return instance;
}

void MessageHandler::set(Message::MessageType mt) {
    std::lock_guard<std::mutex> lock(messageMutex);
    if (currentMessage) {
        if (currentMessage->getType() == mt || currentMessage->getPriority() > Message::getPriority(mt)) {
            return;
        }

        delete currentMessage;
    }

    currentMessage = new Message(mt);
    
}

void MessageHandler::set(Message::MessageType mt, int value) {
    std::lock_guard<std::mutex> lock(messageMutex);
    if (currentMessage) {
        if (currentMessage->getType() == mt) {
            currentMessage->updateValue(value);
            return;
        }

        if (currentMessage->getPriority() > Message::getPriority(mt)) {
            return;
        }

        delete currentMessage;
    }

    currentMessage = new Message(mt);
    currentMessage->updateValue(value);
}

void MessageHandler::remove(Message::MessageType mt) {
    std::lock_guard<std::mutex> lock(messageMutex);
    if (currentMessage && currentMessage->getType() == mt) {
        delete currentMessage;
        currentMessage = nullptr;
    }
}

void MessageHandler::update() {
    std::lock_guard<std::mutex> lock(messageMutex);
    if (currentMessage) {
        if (currentMessage->isExpired()) {
            delete currentMessage;
            currentMessage = nullptr;
        }
    }
}

Message* MessageHandler::getCurrentMessage() {
    std::lock_guard<std::mutex> lock(messageMutex);
    return currentMessage;
}

void MessageHandler::updateProgress(Message::MessageType mt, double value) {
    std::lock_guard<std::mutex> lock(messageMutex);
    if (currentMessage && currentMessage->getType() == mt) {
        currentMessage->updateProgress(value);
    }
}

void MessageHandler::drawCurrentMessage() {
    std::lock_guard<std::mutex> lock(messageMutex);
    if (currentMessage) {
        currentMessage->getOverlay()->draw();
    }
}
