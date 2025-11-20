/*
* Copyright 2016-2025
* This file is copyrighted by onexip GmbH.
* All rights reserved.
*
* Date:   2025-05-05
* Author: Thorsten Knoelker
*/

#ifndef GMSL_CONNECTION_H
#define GMSL_CONNECTION_H

#include "CameraRegisterAccess.h"
#include <thread>
#include <atomic>
#include <chrono>

class GmslConnection {
public:
    GmslConnection();
    void startThread();
    void stopThread();
    bool isConnected();
    void checkConnection();
    bool connect();
    void disconnect();
    void switchCamera(int newCameraId);
    void resetCamera();
    int getSelectedCamera() const;
    void setSelectedCamera(int cameraId);
    void pause();
    void resume();
private:
    static const struct RegValue serializerSetting[];
    static const struct RegValue deserializerSetting[];
    static const struct RegValue cameraSetting[];
    static const struct RegValue deserializerTestPattern[];

    std::atomic<bool> keepRunning;
    std::thread reconnectionThread;
    std::atomic<bool> connected;
    std::atomic<int> selectedCamera;
    std::atomic<bool> pauseReconnection;

    void keepConnection();
    static void initCamera();
    void setVideoPipeSelection();
    bool resetLink();
    static void enableVideoTx();
    static void disableVideoTx();
    static void enableCsiOutput();
    static void disableCsiOutput();
    static bool waitForGmslLink();
    static bool checkVideoLock(bool print = true);
    void startStream();
    static void stopStream();
    void enableMipiPhyLanes();
    static void disableMipiPhyLanes();
    bool activateCamera();
    void enableCameraTestPattern();
    void defaultAllCameraRegisters();
};

#endif
