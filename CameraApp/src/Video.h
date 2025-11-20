/*
* Copyright 2016-2025
* This file is copyrighted by onexip GmbH.
* All rights reserved.
*
* Date:   2025-05-05
* Author: Thorsten Knoelker
*/

#ifndef VIDEO_H
#define VIDEO_H

#ifdef __APPLE__
    #include <OpenGL/gl3.h>
#else
    #ifdef USE_GLES
        #include <gles2.h>
    #else
        #include <gl.h>
    #endif
#endif

#include <gst/gst.h>
#include <gst/app/gstappsink.h>
#include <iostream>
#include <vector>
#include <mutex>
#include <atomic>
#include <chrono>
#include "WhiteBalance.h"

class Video {
public:
    Video();
    ~Video();

    void init();
    void render();
    bool snapshot();
    bool prepareRecording();
    bool startRecording();
    void stopRecording();
    bool finishRecording();
    bool isRecording() const;
    void initGstPipeline();
    void startGstPipeline();
    void stopGstPipeline() const;
    void startWhiteBalance();
    bool frozenForLong();
    void resetFrozenCheck();
    void switchCamera(int newCameraId);
    void setActiveCamera(int cameraId);

    #ifdef DEBUG
    void toggleSimulateVideoFreeze();
    #endif

private:
    struct RecordingElements {
        GstElement* queue = nullptr;
        GstElement* encoder = nullptr;
        GstElement* parser = nullptr;
        GstElement* muxer = nullptr;
        GstElement* filesink = nullptr;
        GstPad* tee_src_pad = nullptr;
    };

    enum class RecordingState {
        Idle,
        Recording,
        Finishing,
        SwitchingBackToNormalStream
    };

    GstElement *pipeline;
    GstElement *appsink;
    unsigned int shaderProgram;
    unsigned int yTextureID;
    unsigned int uvTextureID;
    unsigned int quadVAO;
    unsigned int quadVBO;
    bool glResourcesInitialized;

    std::vector<uint8_t> yTextureData;  // For the Y plane
    std::vector<uint8_t> uvTextureData; // For the UV plane
    size_t ySize;
    size_t uvSize;
    std::mutex textureMutex;
    std::atomic<RecordingState> recordingState = RecordingState::Idle;
    std::atomic<bool> snapshotRequest = false;
    std::string snapshotFilename;
    std::string videoFilename;
    RecordingElements rec;
    std::atomic<int> activeCameraId{0};

    // White balance calibration
    WhiteBalance* whiteBalance;
    WhiteBalance whiteBalance1;
    WhiteBalance whiteBalance2;
    int whiteBalanceSkipFramesCounter = 0;
    GLint locR, locG, locB;
    std::atomic<bool> whiteBalanceUpdateRequest = false;

    // These variables are used to check whether incoming samples have different timestamps
    GstClockTime lastPts;
    unsigned int ptsEqualCount = 0;
    std::atomic<bool> ptsUnchanged = false;

    // Used to check how often the same sample got rendered in order to detect when no new samples arrive anymore
    std::atomic<unsigned int> sampleRenderCount = 0;
    bool videoFrozen = false;
    std::chrono::time_point<std::chrono::high_resolution_clock> frozenTime;
    #ifdef DEBUG
    bool simulateVideoFreeze = false;
    #endif

    void initOpenGLResources();
    static GstFlowReturn newSample(GstAppSink *appsink, gpointer user_data);
    unsigned int createShader(unsigned int type, const char *source) const;
    unsigned int createShaderProgram() const;
    void initWhiteBalanceFactors();
    bool checkFrozen();
    void setFrozen(bool frozen);
    void saveFrame(const GstMapInfo& map) const;
    bool linkRecordingBranchToGstPipeline();
    bool stopRecordingBranchOfGstPipeline();
    void unlinkRecordingBranchFromGstPipeline();
    void restartPipeline() const;
    void applyWhiteBalance();
};

#endif // VIDEO_H
