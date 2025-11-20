/*
* Copyright 2016-2025
* This file is copyrighted by onexip GmbH.
* All rights reserved.
*
* Date:   2025-09-03
* Author: Thorsten Knoelker
*/

#include "IntegrationTesting.h"

IntegrationTesting::IntegrationTesting() {
}

IntegrationTesting::~IntegrationTesting() {
}

IntegrationTesting& IntegrationTesting::getInstance() {
    static IntegrationTesting instance;
    return instance;
}

void IntegrationTesting::setActive(bool active) {
    this->active = active;

    if (!active) {
        temperatureMonitoring = false;
    }
}

bool IntegrationTesting::isActive() const {
    return active;
}

void IntegrationTesting::setTemperatureMonitoring(bool active) {
    this->temperatureMonitoring = active;
}

bool IntegrationTesting::getTemperatureMonitoring() const {
    return temperatureMonitoring;
}
