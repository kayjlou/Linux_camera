/*
* Copyright 2016-2025
* This file is copyrighted by onexip GmbH.
* All rights reserved.
*
* Date:   2025-10-16
* Author: Thorsten Knoelker
*/

#include "Endoscope.h"

bool Endoscope::isAvailable() const {
    return available;
}

void Endoscope::setAvailable(bool available) {
    this->available = available;
    availabilityInitialized = true;
}

bool Endoscope::isAvailabilityInitialized() const {
    return availabilityInitialized;
}

Camera* Endoscope::getCamera() {
    return &camera;
}

LedControl* Endoscope::getLedControl() {
    return &ledControl;
}
