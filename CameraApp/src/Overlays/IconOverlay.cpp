/*
* Copyright 2016-2025
* This file is copyrighted by onexip GmbH.
* All rights reserved.
*
* Date:   2025-05-05
* Author: Thorsten Knoelker
*/

#include "IconOverlay.h"

IconOverlay::IconOverlay() : IconOverlay(Icon::none) {
}

IconOverlay::IconOverlay(Icon icon) : ImageOverlay(getIconFileName(icon)) {
}

std::string IconOverlay::getIconFileName(Icon icon) {
    std::string path = "../data/icons/";

    switch (icon) {
        case Icon::camera:
        path += "camera.png";
        break;
        case Icon::check:
        path += "check.png";
        break;
        case Icon::disconnected:
        path += "disconnected.png";
        break;
        case Icon::warning:
        path += "error.png";
        break;
        case Icon::temperature:
        path += "temperature.png";
        break;
        case Icon::sun:
        path += "sun.png";
        break;
        default:
        return "";
    }

    return path;
}
