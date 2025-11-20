/*
* Copyright 2016-2025
* This file is copyrighted by onexip GmbH.
* All rights reserved.
*
* Date:   2025-05-05
* Author: Thorsten Knoelker
*/

#ifndef OVERLAY_RENDERER_H
#define OVERLAY_RENDERER_H

#ifdef __APPLE__
    #include <OpenGL/gl3.h>  
#else
    #ifdef USE_GLES
        #include <gles2.h>
    #else
        #include <gl.h> 
    #endif
#endif
   

#include <chrono>
#include <imgui.h>
#include <imgui_impl_sdl2.h>
#include <imgui_impl_opengl3.h>
#include "Window.h"
#include "Overlays/RecordingModeOverlay.h"
#include "Overlays/MessageOverlay.h"
#include "WindowUtils.h"
#include "Action.h"
#include <vector>
#include <optional>
#include "MessageHandler.h"
#include "ActionHandler.h"
#include "ActiveCameraOverlay.h"
#include <mutex>

#ifdef DEBUG
#include "DebugOverlay.h"
#include "IntegrationTesting.h"
#include "TemperatureMonitoring.h"
#endif

#ifdef USE_GLES
    #define OPENGL_VERSION "#version 300 es"
#else
    #define OPENGL_VERSION "#version 330"
#endif

class OverlayRenderer {
public:
    ~OverlayRenderer();
    void init(Window* window);
    void update();
    void render() const;
    void shutdown() const;
    void drawOperatingInstructions();
    void setActiveCamera(int cameraId, bool connected);

    #ifdef DEBUG
    void setTemperatureMonitoring(TemperatureMonitoring* temperatureMonitoring);
    #endif
private:
    Window* window;
    RecordingModeOverlay recordingModeOverlay;
    ImageOverlay* operatingInstructions;
    std::chrono::time_point<std::chrono::high_resolution_clock> operatingInstructionsStartTime;
    ActiveCameraOverlay activeCameraOverlay;
    std::mutex activeCameraMutex;

    void newFrame() const;
    void evaluateAction();
    void initOperatingInstructions();

    #ifdef DEBUG
    DebugOverlay* debugOverlay;
    TemperatureMonitoring* temperatureMonitoring;
    #endif
};

#endif
