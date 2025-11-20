/*
* Copyright 2016-2025
* This file is copyrighted by onexip GmbH.
* All rights reserved.
*
* Date:   2025-05-12
* Author: Thorsten Knoelker
*/

#ifndef DEBUG_OVERLAY_H
#define DEBUG_OVERLAY_H

#include "TimedOverlay.h"
#include "TextOverlay.h"
#include <string>

class DebugOverlay : public TimedOverlay {
public:
    DebugOverlay();
    void init() override;
    void draw() override;
    void setText(const std::string& text);
private:
    static constexpr ImU32 textColor = IM_COL32(255, 0, 0, 255);
    static constexpr int AppearanceTimeInMilliseconds = 4000;
    TextOverlay textOverlay;
};

#endif