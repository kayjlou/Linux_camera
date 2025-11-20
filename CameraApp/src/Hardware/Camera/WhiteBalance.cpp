/*
* Copyright 2016-2025
* This file is copyrighted by onexip GmbH.
* All rights reserved.
*
* Date:   2025-05-15
* Author: Thorsten Knoelker
*/

#include "WhiteBalance.h"
#include "Utils.h"
#include "Constants.h"
#include "MessageHandler.h"

WhiteBalance::WhiteBalance() {
    frameCollectionActive = false;
    frameProcessingActive = false;
}

void WhiteBalance::start() {
    printf("WhiteBalance::start\n");
    MessageHandler::getInstance().set(Message::MessageType::WhiteBalance);
    frameCollectionActive = true;
    frameProcessingActive = true;
    collectedFramesCount = 0;
    processedFramesCount = 0;
    redSum = 0.0;
    greenSum = 0.0;
    blueSum = 0.0;
    processImageData();
}

void WhiteBalance::addImageData(const std::vector<uint8_t>& y, const std::vector<uint8_t>& uv) {
    if (!frameCollectionActive) {
        return;
    }

    std::lock_guard<std::mutex> lock(imageDataMutex);
    yData.push(y);
    uvData.push(uv);
    collectedFramesCount++;

    if (collectedFramesCount >= Constants::WhiteBalanceCalibrationTotalFrames) {
        frameCollectionActive = false;
    }
}

void WhiteBalance::processImageData() {
    std::vector<uint8_t> y;
    std::vector<uint8_t> uv;

    while (frameCollectionActive || processedFramesCount < collectedFramesCount) {
        std::this_thread::sleep_for(std::chrono::milliseconds(20));

        {
            std::lock_guard<std::mutex> lock(imageDataMutex);
            if (yData.empty() || uvData.empty()) {
                continue;
            }

            y = yData.front();
            uv = uvData.front();
            yData.pop();
            uvData.pop();
        }

        double redAverage, greenAverage, blueAverage;
        computeAverageRgb(y, uv, redAverage, greenAverage, blueAverage);

        redSum += redAverage;
        greenSum += greenAverage;
        blueSum += blueAverage;
        processedFramesCount++;
        updateProgress();
    }

    calculateRgbFactors();
    clearQueues();
    frameProcessingActive = false;
    MessageHandler::getInstance().remove(Message::MessageType::WhiteBalance);
}

void WhiteBalance::computeAverageRgb(const std::vector<uint8_t>& yPlane, const std::vector<uint8_t>& uvPlane, double& redAverage, double& greenAverage, double& blueAverage) {
    double totalR = 0, totalG = 0, totalB = 0;
    int numPixels = Constants::CameraImageWidth * Constants::CameraImageHeight;

    for (int y = 0; y < Constants::CameraImageHeight; ++y) {
        for (int x = 0; x < Constants::CameraImageWidth; ++x) {
            uint8_t Y = yPlane[y * Constants::CameraImageWidth + x];

            // UV are subsampled 2x2, so divide x and y by 2
            int uvIndex = (y / 2) * (Constants::CameraImageWidth) + (x & ~1); // width because UV stride is full width for interleaved
            uint8_t U = uvPlane[uvIndex];
            uint8_t V = uvPlane[uvIndex + 1];

            // Convert YUV to RGB (BT.601 standard)
            double yValue = static_cast<double>(Y);
            double uValue = static_cast<double>(U) - 128.0;
            double vValue = static_cast<double>(V) - 128.0;

            double r = yValue + 1.402 * vValue;
            double g = yValue - 0.344136 * uValue - 0.714136 * vValue;
            double b = yValue + 1.772 * uValue;

            // Clamp RGB values to [0, 255]
            r = std::min(std::max(r, 0.0), 255.0);
            g = std::min(std::max(g, 0.0), 255.0);
            b = std::min(std::max(b, 0.0), 255.0);

            totalR += r;
            totalG += g;
            totalB += b;
        }
    }

    redAverage = totalR / numPixels;
    greenAverage = totalG / numPixels;
    blueAverage = totalB / numPixels;
}

void WhiteBalance::calculateRgbFactors() {
    printf("WhiteBalance::calculateRgbFactors\n");
    printf("processedFramesCount: %d\n", processedFramesCount);
    double avgRed = redSum / processedFramesCount;
    double avgGreen = greenSum / processedFramesCount;
    double avgBlue = blueSum / processedFramesCount;

    double avgGray = (avgRed + avgGreen + avgBlue) / 3.0;

    red = avgGray / avgRed;
    green = avgGray / avgGreen;
    blue = avgGray / avgBlue;
    printf("red = %.2f\n", red);
    printf("green = %.2f\n", green);
    printf("blue = %.2f\n", blue);
}

double WhiteBalance::getRed() {
    return red;
}

double WhiteBalance::getGreen() {
    return green;
}

double WhiteBalance::getBlue() {
    return blue;
}

bool WhiteBalance::isFrameCollectionActive() const {
    return frameCollectionActive;
}

bool WhiteBalance::isFrameProcessingActive() const {
    return frameProcessingActive;
}

void WhiteBalance::clearQueues() {
    std::lock_guard<std::mutex> lock(imageDataMutex);
    while (!yData.empty()) yData.pop();
    while (!uvData.empty()) uvData.pop();
}

void WhiteBalance::updateProgress() const {
    double progress;

    if (frameProcessingActive) {
        progress =  static_cast<double>(processedFramesCount) / Constants::WhiteBalanceCalibrationTotalFrames;
        progress = std::min(progress, 1.0);
    } else {
        progress = 1.0;
    }

    printf("WhiteBalance::updateProgress: %.2f\n", progress);
    MessageHandler::getInstance().updateProgress(Message::MessageType::WhiteBalance, progress);
}
