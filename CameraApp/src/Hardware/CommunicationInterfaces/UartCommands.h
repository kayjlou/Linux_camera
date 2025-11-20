/*
* Copyright 2016-2025
* This file is copyrighted by onexip GmbH.
* All rights reserved.
*
* Date:   2025-09-25
* Author: Thorsten Knoelker
*/

#ifndef UART_COMMANDS_H
#define UART_COMMANDS_H

enum class InputCommand {
    ButtonEvent = 0x01,
    Heartbeat = 0x04,
    CameraPower = 0x12,
};

enum class OutputCommand {
    HeartbeatResponse = 0x05,
    SimulateDistalLimitSwitch = 0x07,
    SetVideoLive = 0x17,
    GetCameraStatus = 0x1B,
    Error = 0x1F,
};

enum class TargetDevice {
    None = 0x00,
    Handpiece1 = 0x01,
    Handpiece2 = 0x02,
    ControlBoard = 0x03,
    Linux = 0x04,
};

#endif
