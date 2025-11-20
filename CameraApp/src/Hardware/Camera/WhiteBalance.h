/*
* Copyright 2016-2025
* This file is copyrighted by onexip GmbH.
* All rights reserved.
*
* Date:   2025-05-15
* Author: Thorsten Knoelker
*/

#ifndef WHITE_BALANCE_H
#define WHITE_BALANCE_H

#include "CameraRegisterAccess.h"
#include <thread>
#include <atomic>
#include <chrono>
#include <vector>
#include <queue>

class WhiteBalance {
public:
    WhiteBalance();
    void start();
    void addImageData(const std::vector<uint8_t>& y, const std::vector<uint8_t>& uv);
    double getRed();
    double getGreen();
    double getBlue();
    bool isFrameCollectionActive() const;
    bool isFrameProcessingActive() const;
private:
    std::mutex imageDataMutex;
    std::queue<std::vector<uint8_t>> yData;
    std::queue<std::vector<uint8_t>> uvData;
    std::atomic<bool> frameCollectionActive;
    std::atomic<bool> frameProcessingActive;
    int collectedFramesCount = 0;
    int processedFramesCount = 0;
    double redSum = 0.0;
    double greenSum = 0.0;
    double blueSum = 0.0;
    double red = 1.0;
    double green = 1.0;
    double blue = 1.0;

    void processImageData();
    static void computeAverageRgb(const std::vector<uint8_t>& yPlane, const std::vector<uint8_t>& uvPlane, double& redAverage, double& greenAverage, double& blueAverage);
    void calculateRgbFactors();
    void clearQueues();
    void updateProgress() const;
};

#endif