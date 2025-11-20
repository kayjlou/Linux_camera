/*
* Copyright 2016-2025
* This file is copyrighted by onexip GmbH.
* All rights reserved.
*
* Date:   2025-10-20
* Author: Thorsten Knoelker
*/

#ifndef ERROR_HANDLER_H
#define ERROR_HANDLER_H

#include <unordered_set>
#include <mutex>
#include "ErrorCodes.h"

class ErrorHandler {
public:
    ErrorHandler(const ErrorHandler&) = delete;
    ErrorHandler& operator=(const ErrorHandler&) = delete;
    static ErrorHandler& getInstance();

    void addError(int code);
    void removeError(int code);
    bool process();
private:
    std::unordered_set<int> unprocessedErrors;
    std::unordered_set<int> processedErrors;
    std::mutex errorLock;
    
    ErrorHandler();
    ~ErrorHandler();
};

#endif
