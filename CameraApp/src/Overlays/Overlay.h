/*
* Copyright 2016-2025
* This file is copyrighted by onexip GmbH.
* All rights reserved.
*
* Date:   2025-05-05
* Author: Thorsten Knoelker
*/

#ifndef OVERLAY_H
#define OVERLAY_H

#include <imgui.h>

class Overlay {
public:
    virtual ~Overlay() {};

    // init function should contain delayed initializations that cannot be part of 
    // the constructor. For example, if initializations depend on the display window
    // or OpenGL being already setup.
    virtual void init() {};

    virtual void draw() {};

    virtual bool isExpired() const {
        return false;
    };
};

#endif
