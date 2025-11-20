/*
* Copyright 2016-2025
* This file is copyrighted by onexip GmbH.
* All rights reserved.
*
* Date:   2025-08-22
* Author: Thorsten Knoelker
*/

#ifndef UART_COMMUNICATION_H
#define UART_COMMUNICATION_H

#include <string>
#include <thread>
#include <atomic>
#include <termios.h>
#include <vector>
#include "ButtonHandler.h"
#include "UartCommands.h"

class UartCommunication {
public:
    ~UartCommunication();
    void init(ButtonHandler* buttonHandler);
    void setVideoLive(int cameraId, bool live);
    void updateCameraState(int cameraId, bool connected);
    void requestCameraStatus();
    void reportError();

    #ifdef DEBUG
    void simulateDistalLimitSwitch(int cameraId, bool active);
    void simulateCommunicationTimeout();
    #endif
private:
    static inline const std::string devicePath = "/dev/ttyUSB0";
    static constexpr int baudrate = B19200;
    static inline const uint8_t HEADER = 0x5A;
    static inline const uint8_t END = 0xA5;
    static inline const uint8_t ADDRESS = 0x40;
    static constexpr int minimumLength = 6;

    #ifdef DEBUG
    bool forceTimeout = false;
    #endif

    int fd = -1;
    std::atomic<bool> keepRunning{false};
    std::thread uartThread;
    ButtonHandler* buttonHandler;
    std::atomic<int> activeCamera{0};
    std::vector<uint8_t> heartbeatResponsePacket;

    static uint16_t calculateCrc16(const uint8_t* buffer, size_t length);
    static size_t getDataLength(InputCommand command);
    static size_t getDataLength(OutputCommand command);
    void openConnection();
    void closeConnection();
    void run();
    void startThread();
    void stopThread();
    void handle(const uint8_t* buffer, ssize_t length);
    void handle(InputCommand command, const uint8_t* data);
    static Button convertToButton(uint8_t value);
    static int checkForHeader(const uint8_t* buffer, size_t length);
    static int checkForEnd(const uint8_t* buffer, size_t length);
    bool checkHandpieceId(uint8_t id);
    TargetDevice getTargetDevice(int cameraId);
    std::vector<uint8_t> createPacket(OutputCommand command, TargetDevice target, const uint8_t* data);
    void write(std::vector<uint8_t> packet);
    void write(OutputCommand command, TargetDevice target, const uint8_t* data);
};

#endif
