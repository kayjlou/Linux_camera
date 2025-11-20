/*
* Copyright 2016-2025
* This file is copyrighted by onexip GmbH.
* All rights reserved.
*
* Date:   2025-06-11
* Author: Thorsten Knoelker
*/

#include "ImxTemperature.h"

#include <iostream>
#include <fstream>
#include <string>
#include <iomanip>
#include <cmath>

double ImxTemperature::readTemperature() {
    #ifdef SIMULATE_ALL_HARDWARE
    return 11.0;
    #else
    const std::string thermalZonePath = "/sys/class/thermal/thermal_zone0/temp";

    std::ifstream tempFile(thermalZonePath);

    if (!tempFile.is_open()) {
        printf("Error: Could not open thermal zone file\n");
        return -1.0;
    }

    std::string line;

    if (std::getline(tempFile, line)) {
        try {
            int milliCelsius = std::stoi(line);
            return static_cast<double>(milliCelsius) / 1000.0;
        } catch (const std::invalid_argument& e) {
            printf("Error: Invalid argument when converting temperature\n");
        } catch (const std::out_of_range& e) {
            printf("Error: Out of range when converting temperature\n");
        }
    } else {
        printf("Error: Could not read from thermal zone file\n");
    }

    return -1.0;
    #endif
}

std::string ImxTemperature::getTemperatureString() {
    std::string result = "i.mx temp: ";
    double temp = readTemperature();

    if (temp < 0) {
        result += "error";
    } else {
        int rounded = static_cast<int>(std::round(temp));
        result += std::to_string(rounded) + "°C";
    }

    return result;
}
