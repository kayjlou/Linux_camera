/*
* Copyright 2016-2025
* This file is copyrighted by onexip GmbH.
* All rights reserved.
*
* Date:   2025-10-16
* Author: Thorsten Knoelker
*/

#ifndef ENDOSCOPE_H
#define ENDOSCOPE_H

#include "Camera.h"
#include "LedControl.h"

class Endoscope {
public:
    bool isAvailable() const;
    void setAvailable(bool available);
    bool isAvailabilityInitialized() const;
    Camera* getCamera();
    LedControl* getLedControl();
private:
    Camera camera;
    LedControl ledControl;
    bool available = false;
    bool availabilityInitialized = false;
};

#endif
