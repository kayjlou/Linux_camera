/*
* Copyright 2016-2025
* This file is copyrighted by onexip GmbH.
* All rights reserved.
*
* Date:   2025-08-15
* Author: Thorsten Knoelker
*/

#include "I2cCommunication.h"
#include <array>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <thread>
#include <chrono>
#include "Constants.h"

void I2cCommunication::write(Command command, const std::string& data) {
    #ifdef SIMULATE_ALL_HARDWARE
    return;
    #endif

    auto completeCommand = createCompleteCommand(command, data);
    printf("I2cCommunication::write: %s\n", completeCommand.c_str());
    system(completeCommand.data());
}

void I2cCommunication::write(Command command) {
    #ifdef SIMULATE_ALL_HARDWARE
    return;
    #endif

    auto completeCommand = createCompleteCommand(command);
    system(completeCommand.data());
}

std::string I2cCommunication::read(Command command) {
    #ifdef SIMULATE_ALL_HARDWARE
    return "";
    #endif
    
    auto completeCommand = createCompleteCommand(command);
    system(completeCommand.data());
    std::this_thread::sleep_for(std::chrono::milliseconds(Constants::i2cCommunicationDelayInMilliseconds));
    std::string readCommand = "i2cget -y 3 " + deviceAddress + " 2 w";
    
    auto result = executeCommand(readCommand);

    return result;
}

std::string I2cCommunication::executeCommand(const std::string& command) {
    char buffer[128];
    std::string result;
    FILE* pipe = popen(command.c_str(), "r");

    if (!pipe) {
        return "popen() failed!";
    }

    while (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
        result += buffer;
    }

    pclose(pipe);
    return result;
}

std::string I2cCommunication::createCompleteCommand(Command command, const std::string& value) {
    auto commandString = toString(command);
    std::string checksum = calculateChecksum(commandString, value, padding);

    return "i2cset -y 3 " + deviceAddress + " " + header + " " + commandString + " " + value + " " + padding + " " + checksum + " " + end + " i";
}

std::string I2cCommunication::createCompleteCommand(Command command) {
    return createCompleteCommand(command, "0x00");
}

std::string I2cCommunication::toString(Command command) {
    switch (command) {
        case Command::LedBrightness:
            return "0x05";
        case Command::GetTemperature:
            return "0x07";
        default:
            return "--";
    }
}

std::string I2cCommunication::calculateChecksum(const std::string& command, const std::string& data, const std::string& padding) {
    uint8_t commandValue = static_cast<uint8_t>(std::stoi(command, nullptr, 0));
    uint8_t dataValue = static_cast<uint8_t>(std::stoi(data, nullptr, 0));
    uint8_t paddingValue = static_cast<uint8_t>(std::stoi(padding, nullptr, 0));

    std::array<uint8_t, 3> values = {commandValue, dataValue, paddingValue};
    uint8_t checksum = calculateChecksum(values.data(), values.size());

    return toString(checksum);
}

uint8_t I2cCommunication::calculateChecksum(const uint8_t* data, size_t length) {
    if (!data || length == 0) {
        return 0;
    }

    uint8_t checksum = 0;

    for (size_t i = 0; i < length; ++i) {
        checksum ^= data[i];
    }

    return checksum & 0xFF; // Ensure checksum is within 8 bits
}

std::string I2cCommunication::toString(uint8_t value) {
    std::ostringstream oss;
    oss << "0x" << std::uppercase << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(value);

    return oss.str();
}
