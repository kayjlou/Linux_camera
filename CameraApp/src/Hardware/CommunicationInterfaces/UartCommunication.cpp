/*
* Copyright 2016-2025
* This file is copyrighted by onexip GmbH.
* All rights reserved.
*
* Date:   2025-08-22
* Author: Thorsten Knoelker
*/

#include "UartCommunication.h"
#include <array>
#include <iostream>
#include <poll.h>
#include <fcntl.h>
#include <vector>
#include <unistd.h>
#include "Constants.h"
#include "Versions.h"
#include <cstring>
#include "ActionHandler.h"
#include <iomanip>
#include "Message.h"
#include "MessageHandler.h"

#ifndef SIMULATE_ALL_HARDWARE
#include <linux/serial.h>
#endif

UartCommunication::~UartCommunication() {
    stopThread();
    closeConnection();
}

void UartCommunication::init(ButtonHandler* buttonHandler) {
    this->buttonHandler = buttonHandler;
    uint8_t data[1];
    data[0] = static_cast<uint8_t>(TargetDevice::Linux);
    heartbeatResponsePacket = createPacket(OutputCommand::HeartbeatResponse, TargetDevice::ControlBoard, data);
    #ifndef SIMULATE_ALL_HARDWARE
    openConnection();
    startThread();
    #endif
}

void UartCommunication::openConnection() {
    fd = open(devicePath.c_str(), O_RDWR | O_NOCTTY | O_NONBLOCK);

    if (fd < 0) {
        printf("Error opening UART device %s\n", devicePath.c_str());
    }

    struct termios tty{};
    if (tcgetattr(fd, &tty) != 0) {
        printf("Failed to get UART attributes\n");
        close(fd);
        fd = -1;
        return;
    }

    cfsetospeed(&tty, baudrate);
    cfsetispeed(&tty, baudrate);

    tty.c_cflag = (tty.c_cflag & ~CSIZE) | CS8;     // 8-bit chars
    tty.c_cflag |= (CLOCAL | CREAD);                // ignore modem controls, enable reading
    tty.c_cflag &= ~(PARENB | PARODD);              // shut off parity
    tty.c_cflag &= ~CSTOPB;                         // 1 stop bit
    tty.c_cflag &= ~CRTSCTS;                        // no hardware flow control

    tty.c_iflag = 0;                                // no special input processing
    tty.c_lflag = 0;                                // no signaling chars, no echo
    tty.c_oflag = 0;                                // no remapping, no delays

    tty.c_cc[VMIN]  = 1;                            // read blocks until at least 1 byte
    tty.c_cc[VTIME] = 0;                            // no inter-character timer

    if (tcsetattr(fd, TCSANOW, &tty) != 0) {
        printf("Failed to set UART attributes\n");
        close(fd);
        fd = -1;
        return;
    }
}

void UartCommunication::closeConnection() {
    if (fd != -1) {
        close(fd);
        fd = -1;
    }
}

void UartCommunication::startThread() {
    keepRunning = true;
    uartThread = std::thread(&UartCommunication::run, this);
}

void UartCommunication::stopThread() {
    keepRunning = false;

    if (uartThread.joinable()) {
        uartThread.join();
    }
}

#ifdef DEBUG
void UartCommunication::simulateCommunicationTimeout() {
    forceTimeout = !forceTimeout;
}
#endif

void UartCommunication::run() {
    constexpr int maxBufferSize = 64;
    struct pollfd fds[1];
    fds[0].fd = fd;
    fds[0].events = POLLIN;
    std::vector<uint8_t> buffer(maxBufferSize);
    int startIndex = -1;
    int endIndex = -1;
    ssize_t totalBytesRead = 0;

    while (keepRunning) {
        int ret = poll(fds, 1, -1);

        //TODO: Keep this in case a heartbeat gets implemented and timeout should be handled/simulated.
        /*int ret = poll(fds, 1, 200);

        #ifdef DEBUG
        if (forceTimeout) {
            ret = 0;
        }
        #endif*/

        if (ret > 0 && (fds[0].revents & POLLIN)) {
            ssize_t n = 0;

            if (startIndex >= 0) {
                n = read(fd, buffer.data() + totalBytesRead, buffer.size() - totalBytesRead);
            } else {
                n = read(fd, buffer.data(), buffer.size());
            }

            if (n > 0) {
                totalBytesRead += n;
                startIndex = checkForHeader(buffer.data(), totalBytesRead);
                endIndex = checkForEnd(buffer.data(), totalBytesRead);

/*#ifdef DEBUG
                printf("Received (%zd bytes): ", n);
                for (ssize_t i = 0; i < totalBytesRead; ++i) {
                    printf("%02x ", (unsigned char)buffer[i]);
                }
                printf("\n");
#endif*/
            }

            while (startIndex >= 0 && endIndex > startIndex) {
                std::vector<uint8_t> subBuffer(buffer.begin() + startIndex, buffer.begin() + endIndex + 1);
                handle(subBuffer.data(), subBuffer.size());

                size_t newStart = endIndex + 1;
                size_t remaining = totalBytesRead - newStart;
                if (remaining > 0) {
                    std::memmove(buffer.data(), buffer.data() + newStart, remaining);
                }

                totalBytesRead = remaining;
                startIndex = checkForHeader(buffer.data(), totalBytesRead);
                endIndex = checkForEnd(buffer.data(), totalBytesRead);
            }
        } else if (ret < 0) {
            printf("Error during UART poll\n");
            keepRunning = false;
        //} else if (ret == 0) { //TODO: Keep this in case a heartbeat gets implemented and timeout should be handled/simulated.
        //    MessageHandler::getInstance().set(Message::MessageType::UartTimeout);
        }
    }
}


int UartCommunication::checkForHeader(const uint8_t* buffer, size_t length) {
    for (ssize_t i = 0; i < length; ++i) {
        if (buffer[i] == HEADER) {
            return i;
        }
    }

    return -1;
}

int UartCommunication::checkForEnd(const uint8_t* buffer, size_t length) {
    for (ssize_t i = 0; i < length; ++i) {
        if (buffer[i] == END) {
            return i;
        }
    }

    return -1;
}

void UartCommunication::handle(const uint8_t* buffer, ssize_t length) {
    if (!buffer) {
        return;
    }

    if (length < minimumLength ) {
        return;
    }

    uint8_t header = buffer[0];
    uint8_t address = buffer[1];
    uint8_t commandRaw = buffer[2];
    InputCommand command = static_cast<InputCommand>(commandRaw);
    size_t dataLength = getDataLength(command);

    if (length < minimumLength + dataLength) {
        return;
    }

    uint8_t data[dataLength];
    memcpy(data, &buffer[3], dataLength);

    uint16_t receivedCrc = (static_cast<uint16_t>(buffer[3 + dataLength]) << 8) | buffer[4 + dataLength];
    uint8_t end = buffer[5 + dataLength];

    if (header != HEADER || end != END) {
        return;
    }

    uint8_t crcBuffer[3 + dataLength];
    crcBuffer[0] = header;
    crcBuffer[1] = address;
    crcBuffer[2] = commandRaw;
    memcpy(&crcBuffer[3], &buffer[3], dataLength);

    auto calculatedCrc = calculateCrc16(crcBuffer, sizeof(crcBuffer));

    if (calculatedCrc != receivedCrc) {
        printf("CRC mismatch: calculated 0x%04X, received 0x%04X\n", calculatedCrc, receivedCrc);
        return;
    }

    handle(command, data);
}

void UartCommunication::handle(InputCommand command, const uint8_t* data) {
    #ifdef DEBUG
    if (command != InputCommand::Heartbeat) {
        printf("Handling command %d\n", static_cast<int>(command));
    }
    #endif

    switch (command) {
        case InputCommand::ButtonEvent:
            if (checkHandpieceId(data[0]) && buttonHandler) {
                buttonHandler->handle(data[1], data[2]);
            }
            break;
        case InputCommand::CameraPower:
        {
            int cameraId = data[0];
            bool attached = data[1] == 1;
            auto action = Action(Action::ActionType::CameraAvailability, cameraId, attached);
            ActionHandler::getInstance().addAction(action);
            break;
        }
        case InputCommand::Heartbeat:
            write(heartbeatResponsePacket);
            break;
        default:
            break;
    }
}

void UartCommunication::write(std::vector<uint8_t> packet) {
    if (fd != -1) {
        ssize_t written = ::write(fd, packet.data(), packet.size());
        if (written < 0) {
            printf("UART write failed");
        }
    }
}

void UartCommunication::write(OutputCommand command, TargetDevice target, const uint8_t* data) {
    if (target == TargetDevice::None) {
        return;
    }

    auto packet = createPacket(command, target, data);
    write(packet);
}

std::vector<uint8_t> UartCommunication::createPacket(OutputCommand command, TargetDevice target, const uint8_t* data) {
    size_t dataLength = getDataLength(command);
    uint8_t address = static_cast<uint8_t>(target);

    std::vector<uint8_t> crcBuffer(3 + dataLength);
    crcBuffer[0] = HEADER;
    crcBuffer[1] = address;
    crcBuffer[2] = static_cast<uint8_t>(command);
    memcpy(&crcBuffer[3], data, dataLength);

    uint16_t crc = calculateCrc16(crcBuffer.data(), crcBuffer.size());

    std::vector<uint8_t> frame(minimumLength + dataLength);
    frame[0] = HEADER;
    frame[1] = address;
    frame[2] = static_cast<uint8_t>(command);
    memcpy(&frame[3], data, dataLength);
    frame[3 + dataLength] = (crc >> 8) & 0xFF; // CRC high
    frame[4 + dataLength] = crc & 0xFF;        // CRC low
    frame[5 + dataLength] = END;

    return frame;
}

uint16_t UartCommunication::calculateCrc16(const uint8_t* buffer, size_t length) {
    uint16_t crc = 0xFFFF;

    // Include protocol signature in CRC calculation
    const uint8_t *sig = (const uint8_t *)PROTO_SIG_VERSION;
    size_t sigLength = strlen(PROTO_SIG_VERSION);

    // Process signature first
    for (size_t i = 0; i < sigLength; ++i) {
        crc ^= (uint16_t)sig[i] << 8;

        for (uint8_t j = 0; j < 8; ++j) {
            if (crc & 0x8000) {
                crc = (crc << 1) ^ 0x8005;
            } else {
                crc <<= 1;
            }
        }
    }

    // Process data
    for (size_t i = 0; i < length; ++i) {
        crc ^= (uint16_t)buffer[i] << 8;

        for (uint8_t j = 0; j < 8; ++j) {
            if (crc & 0x8000) {
                crc = (crc << 1) ^ 0x8005;
            } else {
                crc <<= 1;
            }
        }
    }

    return crc;
}

size_t UartCommunication::getDataLength(InputCommand command) {
    switch (command)
    {
    case InputCommand::CameraPower:
        return 2;
    case InputCommand::ButtonEvent:
        return 3;
    default:
        return 0;
    }
}

size_t UartCommunication::getDataLength(OutputCommand command) {
    switch (command)
    {
    case OutputCommand::GetCameraStatus:
    case OutputCommand::Error: // Currently no error codes are used here. Control board will only be informed that app detected an error.
        return 0;
    case OutputCommand::HeartbeatResponse:
    case OutputCommand::SetVideoLive:
    case OutputCommand::SimulateDistalLimitSwitch:
        return 1;
    default:
        return 0;
    }
}

Button UartCommunication::convertToButton(uint8_t value) {
    switch (value) {
        case 0:
            return Button::Top;
        case 1:
            return Button::Right;
        case 2:
            return Button::Bottom;
        case 3:
            return Button::Left;
        default:
            return Button::None;
    }
}

void UartCommunication::updateCameraState(int cameraId, bool connected) {
    if (connected) {
        if (activeCamera != cameraId) {
            setVideoLive(activeCamera, false);
        }

        setVideoLive(cameraId, true);
        activeCamera = cameraId;
    } else {
        activeCamera = 0;
        setVideoLive(1, false);
        setVideoLive(2, false);
    }
}

bool UartCommunication::checkHandpieceId(uint8_t id) {
    return id == activeCamera;
}

void UartCommunication::setVideoLive(int cameraId, bool live) {
    TargetDevice target;

    if (cameraId == 1) {
        target = TargetDevice::Handpiece1;
    } else if (cameraId == 2) {
        target = TargetDevice::Handpiece2;
    } else {
        return;
    }

    uint8_t data[1];
    data[0] = live ? 1 : 0;
    write(OutputCommand::SetVideoLive, target, data);
}

void UartCommunication::requestCameraStatus() {
    write(OutputCommand::GetCameraStatus, TargetDevice::ControlBoard, nullptr);
}

void UartCommunication::reportError() {
    write(OutputCommand::Error, TargetDevice::ControlBoard, nullptr);
}

TargetDevice UartCommunication::getTargetDevice(int cameraId) {
    if (cameraId == 1) {
        return TargetDevice::Handpiece1;
    } else if (cameraId == 2) {
        return TargetDevice::Handpiece2;
    } else {
        return TargetDevice::None;
    }
}

#ifdef DEBUG
void UartCommunication::simulateDistalLimitSwitch(int cameraId, bool active) {
    uint8_t data[1];
    data[0] = active ? 1 : 0;
    write(OutputCommand::SimulateDistalLimitSwitch, getTargetDevice(cameraId), data);
    printf("SimulateDistalLimitSwitch: cameraId=%d, active=%d\n", cameraId, active);
}
#endif
