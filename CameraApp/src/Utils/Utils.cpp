/*
* Copyright 2016-2025
* This file is copyrighted by onexip GmbH.
* All rights reserved.
*
* Date:   2025-05-05
* Author: Thorsten Knoelker
*/

#include "Utils.h"
#include <numeric>
#include <cmath>
#include <unistd.h>

void Utils::msleep(unsigned int msecond) {
	usleep(msecond*1000);
}

int Utils::average(const std::vector<int>& values) {
	double avg = static_cast<double>(std::accumulate(values.begin(), values.end(), 0)) / values.size();

    return static_cast<int>(std::round(avg));
}

int64_t Utils::timePassedSince(const std::chrono::time_point<std::chrono::high_resolution_clock>& timePoint) {
	auto currentTime = std::chrono::high_resolution_clock::now();
	auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - timePoint);
	return duration.count();
}
