/*
* Copyright 2016-2025
* This file is copyrighted by onexip GmbH.
* All rights reserved.
*
* Date:   2025-07-21
* Author: Thorsten Knoelker
*/

#include "Versions.h"

std::string Versions::getCompleteVersionString() {
    return "App Version: " + Versions::getAppVersion() + "\n" + 
    "Yocto Linux Version: " + Versions::getYoctoLinuxVersion();
}

std::string Versions::getAppVersion() {
    return APP_VERSION;
}

std::string Versions::getYoctoLinuxVersion() {
    return YOCTO_LINUX_VERSION;
}

