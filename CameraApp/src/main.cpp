/*
 * Copyright 2016-2025
 * This file is copyrighted by onexip GmbH.
 * All rights reserved.
 *
 * Date:   2025-05-05
 * Author: Thorsten Knoelker
 */

#ifndef __APPLE__
#ifdef USE_GLES
#define GLAD_GLES2_IMPLEMENTATION
#include <gles2.h>
#else
#define GLAD_GL_IMPLEMENTATION
#include <gl.h>
#endif
#endif

#include <gst/gst.h>
#include "OverlayRenderer.h"
#include "Video.h"
#include "ActionHandler.h"
#include "MessageHandler.h"
#include "Utils.h"
#include "ButtonHandler.h"
#include "ControlBoard.h"
#include "ErrorHandler.h"

constexpr int operatingInstructionsVisibleTime = 7000;
constexpr int cameraStatusRequestRepeatInterval = 1000;
constexpr int gmslConnectionInitDelay = 3000;
constexpr int displayFps = 30;

ButtonHandler buttonHandler;
Video video;
OverlayRenderer overlayRenderer;
ControlBoard controlBoard;

//int counter = 0; // Keep this in case the framerate needs to be measured again
SDL_Window* sdlWindow = nullptr;
bool gmslConnectionActive;
Uint64 targetFrameTime;

enum class AppState {
    InitApp,
    WaitForHardwareInitializations,
    ShowInstructionScreen,
    StartVideo,
    Running
};

void updateCameraState(bool connected) {
    int camId = controlBoard.getActiveEndoscopeId();
    controlBoard.getUartCommunication()->updateCameraState(camId, connected);
    video.setActiveCamera(camId);
    overlayRenderer.setActiveCamera(camId, connected);
}

void initApp(Window* window) {
    overlayRenderer.init(window);
    video.init();

    ActionHandler::getInstance().setControlBoard(&controlBoard);
    ActionHandler::getInstance().setVideo(&video);

    glClearColor(0.6f, 0.6f, 0.6f, 1.0f);
    sdlWindow = window->getSdlWindow();
    targetFrameTime = SDL_GetPerformanceFrequency() / displayFps;

    controlBoard.initCommunication(&buttonHandler);
}

void delayToAchieveTargetDisplayFps(Uint64 frameStart) {
    Uint64 frameEnd = SDL_GetPerformanceCounter();
    Uint64 frameDuration = frameEnd - frameStart;

    if (frameDuration < targetFrameTime) {
        Uint32 delayMs = (Uint32)((targetFrameTime - frameDuration) * 1000 / SDL_GetPerformanceFrequency());
        SDL_Delay(delayMs);
    }
}

void showInstructionScreen() {
    Uint64 frameStart = SDL_GetPerformanceCounter();
    glClear(GL_COLOR_BUFFER_BIT);

    overlayRenderer.drawOperatingInstructions();
    overlayRenderer.render();

    SDL_GL_SwapWindow(sdlWindow);
    delayToAchieveTargetDisplayFps(frameStart);
}

void startVideo() {
    gmslConnectionActive = controlBoard.checkGmslConnection(true);

    if (gmslConnectionActive) {
        video.startGstPipeline();
    }

    if (gmslConnectionActive) {
        MessageHandler::getInstance().set(Message::MessageType::CameraConnectedInitially);
    } else {
        MessageHandler::getInstance().set(Message::MessageType::CameraDisconnected);
    }

    updateCameraState(gmslConnectionActive);

#ifdef DEBUG
    overlayRenderer.setTemperatureMonitoring(controlBoard.getTemperatureMonitoring());
#endif
}

void processErrors() {
    if (ErrorHandler::getInstance().process()) {
        controlBoard.getUartCommunication()->reportError();
    }
}

void process(Window* window) {
    Uint64 frameStart = SDL_GetPerformanceCounter();
    glClear(GL_COLOR_BUFFER_BIT);

    bool connected = controlBoard.checkGmslConnection();

    if (connected) {
        if (!gmslConnectionActive) { // newly connected or reconnected camera
            video.startGstPipeline();
            updateCameraState(connected);
        }

        video.render();
    
        if (video.frozenForLong()
            && controlBoard.getActiveEndoscope()->isAvailable()) {
            #ifndef SIMULATE_ALL_HARDWARE
            ActionHandler::getInstance().addAction(Action::ActionType::ResetCamera);
            #endif
        }
    } else {
        if (gmslConnectionActive) { //camera got disconnected
            video.stopGstPipeline();
            updateCameraState(connected);
        }
    }

    gmslConnectionActive = connected;

    buttonHandler.evaluate();
    processErrors();
    overlayRenderer.update();
    overlayRenderer.render();

    SDL_GL_SwapWindow(sdlWindow);

    // Keep this in case the framerate needs to be measured again
    /*if (counter > 20) {
        printf("FPS: %.1f\n", ImGui::GetIO().Framerate);
        counter = 0;
    } else {
        counter++;
    }*/

    delayToAchieveTargetDisplayFps(frameStart);
}

int main() {
    Window window;

    if (!window.init(&buttonHandler)) {
        return -1;
    }

    if (!window.open()) {
        return -1;
    }

    AppState appState = AppState::InitApp;
    std::chrono::time_point<std::chrono::high_resolution_clock> showInstructionsStartTime;

    while (!window.shouldClose()) {
        switch(appState) {
            case AppState::InitApp:
                initApp(&window);
                showInstructionsStartTime = std::chrono::high_resolution_clock::now();
                controlBoard.getUartCommunication()->requestCameraStatus();
                #ifdef SIMULATE_ALL_HARDWARE
                controlBoard.getEndoscope(1)->setAvailable(true);
                controlBoard.getEndoscope(2)->setAvailable(true);
                #endif
                appState = AppState::WaitForHardwareInitializations;
                break;
            case AppState::WaitForHardwareInitializations:
            {
                if (controlBoard.isAvailabilityInitialized()) {
                    controlBoard.initGmslConnection();
                    appState = AppState::ShowInstructionScreen;
                } else {
                    auto timePassed = Utils::timePassedSince(showInstructionsStartTime);

                    if (timePassed < gmslConnectionInitDelay) {
                        // While waiting for availability feedback, show the instruction screen!
                        showInstructionScreen();
                    } else {
                        printf("Timeout waiting for endoscope availability initialization, init GMSL connection anyway\n");
                        controlBoard.initGmslConnection();
                        appState = AppState::ShowInstructionScreen;
                    }
                }

                break;
            }
            case AppState::ShowInstructionScreen:
            {
                auto timePassed = Utils::timePassedSince(showInstructionsStartTime);

                if (timePassed < operatingInstructionsVisibleTime) {
                    showInstructionScreen();
                } else {
                    appState = AppState::StartVideo;
                }
                break;
            }
            case AppState::StartVideo:
                startVideo();
                appState = AppState::Running;
                break;
            case AppState::Running:
                process(&window);
                break;            
        }
    }

    sdlWindow = nullptr;
    overlayRenderer.shutdown();
    window.close();

    return 0;
}
