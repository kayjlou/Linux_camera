/*
* Copyright 2016-2025
* This file is copyrighted by onexip GmbH.
* All rights reserved.
*
* Date:   2025-09-03
* Author: Thorsten Knoelker
*/

#ifndef INTEGRATION_TESTING_H
#define INTEGRATION_TESTING_H

#include <atomic>

class IntegrationTesting {
public:
    IntegrationTesting(const IntegrationTesting&) = delete;
    IntegrationTesting& operator=(const IntegrationTesting&) = delete;
    static IntegrationTesting& getInstance();

    void setActive(bool active);
    bool isActive() const;
    void setTemperatureMonitoring(bool active);
    bool getTemperatureMonitoring() const;
private:
    std::atomic<bool> active{false};
    std::atomic<bool> temperatureMonitoring{false};

    IntegrationTesting();
    ~IntegrationTesting();
};

#endif

