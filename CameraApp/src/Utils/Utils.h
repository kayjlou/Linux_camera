/*
* Copyright 2016-2025
* This file is copyrighted by onexip GmbH.
* All rights reserved.
*
* Date:   2025-05-05
* Author: Thorsten Knoelker
*/

#ifndef UTILS_H
#define UTILS_H

#include <vector>
#include <chrono>

class Utils {
public:
    static void msleep(unsigned int msecond);
    static int average(const std::vector<int>& values);
    static int64_t timePassedSince(const std::chrono::time_point<std::chrono::high_resolution_clock>& timePoint);
};

#endif