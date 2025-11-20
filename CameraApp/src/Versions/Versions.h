/*
* Copyright 2016-2025
* This file is copyrighted by onexip GmbH.
* All rights reserved.
*
* Date:   2025-07-21
* Author: Thorsten Knoelker
*/

#ifndef VERSIONS_H
#define VERSIONS_H

#define APP_VERSION "1.0.0"
#define YOCTO_LINUX_VERSION "1.0.0"
#define PROTO_SIG_VERSION "PROTO_V1"

#include <string>

class Versions {
public:
    static std::string getCompleteVersionString();
    static std::string getAppVersion();
    static std::string getYoctoLinuxVersion();
};

#endif
