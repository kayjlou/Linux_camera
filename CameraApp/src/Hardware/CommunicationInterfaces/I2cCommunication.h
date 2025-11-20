/*
* Copyright 2016-2025
* This file is copyrighted by onexip GmbH.
* All rights reserved.
*
* Date:   2025-08-15
* Author: Thorsten Knoelker
*/

#ifndef I2C_COMMUNICATION_H
#define I2C_COMMUNICATION_H

#include <string>

class I2cCommunication {
public:
    enum class Command {
        LedBrightness,
        GetTemperature,
    };

    static void write(Command command, const std::string& data);
    static void write(Command command);
    static std::string read(Command command);
private:
    static inline const std::string deviceAddress = "0x3A";
    static inline const std::string header = "0x5A";
    static inline const std::string end = "0xA5";
    static inline const std::string padding = "0x00";

    static std::string createCompleteCommand(Command command, const std::string& data);
    static std::string createCompleteCommand(Command command);
    static std::string calculateChecksum(const std::string& command, const std::string& data, const std::string& padding);
    static uint8_t calculateChecksum(const uint8_t* data, size_t length);
    static std::string toString(Command command);
    static std::string toString(uint8_t value);
    static std::string executeCommand(const std::string& command);
};

#endif
