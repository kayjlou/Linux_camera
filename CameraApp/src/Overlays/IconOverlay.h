/*
* Copyright 2016-2025
* This file is copyrighted by onexip GmbH.
* All rights reserved.
*
* Date:   2025-05-05
* Author: Thorsten Knoelker
*/

#ifndef ICON_H
#define ICON_H

#include "ImageOverlay.h"

enum class Icon {
    none,
    camera,
    check,
    disconnected,
    warning,
    temperature,
    sun
};

class IconOverlay : public ImageOverlay {
public:
    IconOverlay();
    IconOverlay(Icon icon);
private:
    static std::string getIconFileName(Icon icon);
};

#endif
