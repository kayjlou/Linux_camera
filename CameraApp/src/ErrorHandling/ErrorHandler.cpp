/*
* Copyright 2016-2025
* This file is copyrighted by onexip GmbH.
* All rights reserved.
*
* Date:   2025-10-20
* Author: Thorsten Knoelker
*/

#include "ErrorHandler.h"

ErrorHandler::ErrorHandler() {}

ErrorHandler::~ErrorHandler() {}

ErrorHandler& ErrorHandler::getInstance() {
    static ErrorHandler instance;
    return instance;
}

void ErrorHandler::addError(int code) {
    std::lock_guard<std::mutex> lock(errorLock);
    unprocessedErrors.insert(code);
}

void ErrorHandler::removeError(int code) {
    std::lock_guard<std::mutex> lock(errorLock);
    unprocessedErrors.erase(code);
    processedErrors.erase(code);
}

bool ErrorHandler::process() {
    std::lock_guard<std::mutex> lock(errorLock);
    bool hasErrors = !unprocessedErrors.empty();

    for (int code : unprocessedErrors) {
        if (processedErrors.find(code) != processedErrors.end()) {
            // Error code has already been processed
            continue;
        } else {
            processedErrors.insert(code);
        }
    }

    unprocessedErrors.clear();

    return hasErrors;
}
