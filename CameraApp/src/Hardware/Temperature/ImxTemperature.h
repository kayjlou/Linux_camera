/*
* Copyright 2016-2025
* This file is copyrighted by onexip GmbH.
* All rights reserved.
*
* Date:   2025-06-11
* Author: Thorsten Knoelker
*/

#ifndef IMX_TEMPERATURE_H
#define IMX_TEMPERATURE_H

#include <string>

class ImxTemperature {
public:
    static double readTemperature();
    static std::string getTemperatureString();
};

#endif
