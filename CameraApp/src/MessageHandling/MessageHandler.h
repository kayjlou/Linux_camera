/*
* Copyright 2016-2025
* This file is copyrighted by onexip GmbH.
* All rights reserved.
*
* Date:   2025-05-05
* Author: Thorsten Knoelker
*/

#ifndef MESSAGE_HANDLER_H
#define MESSAGE_HANDLER_H

#include "Message.h"
#include <mutex>

class MessageHandler {
public:
    MessageHandler(const MessageHandler&) = delete;
    MessageHandler& operator=(const MessageHandler&) = delete;
    static MessageHandler& getInstance();

    void set(Message::MessageType mt);
    void set(Message::MessageType mt, int value);
    void remove(Message::MessageType mt);
    void updateProgress(Message::MessageType mt, double value);
    void update();
    Message* getCurrentMessage();
    void drawCurrentMessage();
private:
    Message* currentMessage = nullptr;
    std::mutex messageMutex;

    MessageHandler();
    ~MessageHandler();
};

#endif
