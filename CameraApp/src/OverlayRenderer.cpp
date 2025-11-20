/*
* Copyright 2016-2025
* This file is copyrighted by onexip GmbH.
* All rights reserved.
*
* Date:   2025-05-05
* Author: Thorsten Knoelker
*/

#include "OverlayRenderer.h"
#include "Fonts.h"
#include "BrightnessOverlay.h"
#include "WhiteBalanceOverlay.h"

OverlayRenderer::~OverlayRenderer() {
    #ifdef DEBUG
    if (debugOverlay) {
        delete debugOverlay;
    }
    #endif
}

void OverlayRenderer::init(Window* window) {
    this->window = window;
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui_ImplSDL2_InitForOpenGL(window->getSdlWindow(), window->getSdlContext());
    ImGui_ImplOpenGL3_Init(OPENGL_VERSION);
    
    int width, height;
    window->getSize(&width, &height);
    printf("Window width=%d\n", width);
    printf("Window height=%d\n", height);
    WindowUtils::getInstance().setWindowWidth(width);
    WindowUtils::getInstance().setWindowHeight(height);

    Fonts::getInstance().loadFonts();

    recordingModeOverlay.initPositions();
    activeCameraOverlay.init();
    initOperatingInstructions();

    #ifdef DEBUG
    debugOverlay = new DebugOverlay();
    debugOverlay->init();
    #endif
}

void OverlayRenderer::evaluateAction() {
    auto action = ActionHandler::getInstance().getHandledAction();

    if (!action.has_value()) {
        return;
    }

    auto a = action.value();
    
    switch (a.getActionType()) {
        case Action::ActionType::IncreaseBrightness:
        case Action::ActionType::DecreaseBrightness:
        MessageHandler::getInstance().set(Message::MessageType::Gain, a.getResultInt());
        break;
        case Action::ActionType::IncreaseLedBrightness:
        case Action::ActionType::DecreaseLedBrightness:
        MessageHandler::getInstance().set(Message::MessageType::LedBrightness, a.getResultInt());
        break;
        case Action::ActionType::Snapshot:
        if (a.getResultInt() > 0) {
            recordingModeOverlay.setMode(RecordingModeOverlay::RecordingMode::Shot);
        }
        break;
        case Action::ActionType::PrepareVideoRecording:
        if (a.getResultInt() > 0) {
            MessageHandler::getInstance().set(Message::MessageType::RecordingAboutToStart);
        }
        break;
        case Action::ActionType::StartVideoRecording:
        if (a.getResultInt() > 0) {
            recordingModeOverlay.setMode(RecordingModeOverlay::RecordingMode::Recording);
        }
        break;
        case Action::ActionType::StopVideoRecording:
        MessageHandler::getInstance().set(Message::MessageType::SavingVideo);
        recordingModeOverlay.setMode(RecordingModeOverlay::RecordingMode::None);
        break;
        case Action::ActionType::SavingVideoRecording:
        MessageHandler::getInstance().remove(Message::MessageType::SavingVideo);
        if (a.getResultInt() > 0) {
            MessageHandler::getInstance().set(Message::MessageType::SavingVideoSuccessful);
        } else {
            MessageHandler::getInstance().set(Message::MessageType::SavingVideoFailed);
        }
        break;
        case Action::ActionType::PrepareCameraSwitching:
        activeCameraOverlay.setSwitchingInProgress();
        break;
        case Action::ActionType::SwitchCamera:
        {
            std::lock_guard<std::mutex> lock(activeCameraMutex);
            activeCameraOverlay.setActiveCamera(a.getResultInt());
            MessageHandler::getInstance().remove(Message::MessageType::Gain);
            MessageHandler::getInstance().remove(Message::MessageType::LedBrightness);
        }
        break;
        case Action::ActionType::CameraAvailability:
        activeCameraOverlay.setCameraAvailability(a.getCameraId(), a.getActive());
        break;
        #ifdef DEBUG
        case Action::ActionType::ReadImxTemperature:
        if (debugOverlay) {
            debugOverlay->setText(a.getResultString());
        }
        break;
        case Action::ActionType::ShowVersions:
        if (debugOverlay) {
            debugOverlay->setText(a.getResultString());
        }
        break;
        case Action::ActionType::IntegrationTestModeToggle:
        if (debugOverlay) {
            bool active = IntegrationTesting::getInstance().isActive();
            debugOverlay->setText("Integration Test Mode: " + std::string(active ? "ON" : "OFF"));
        }
        break;
        case Action::ActionType::DisplayTemperatureMonitoringToggle:
        if (debugOverlay && a.getResultInt() == 0) {
            debugOverlay->setText("");
        }
        break;
        case Action::ActionType::SimulateDistalLimitSwitch:
        if (debugOverlay) {
            debugOverlay->setText("Set limit switch of camera " + std::to_string(a.getCameraId()) + ": " + std::string(a.getActive() ? "ON" : "OFF"));
        }
        break;
        #endif
        default:
        break;
    }
}

void OverlayRenderer::drawOperatingInstructions() {
    newFrame();
    operatingInstructions->draw();
}

void OverlayRenderer::update() {
    evaluateAction();
    newFrame();
    recordingModeOverlay.draw();
    activeCameraOverlay.draw();
    MessageHandler::getInstance().update();
    MessageHandler::getInstance().drawCurrentMessage();

    #ifdef DEBUG
    if (debugOverlay) {
        if (IntegrationTesting::getInstance().getTemperatureMonitoring()) {
            debugOverlay->setText(temperatureMonitoring->getTemperatureText());
        }

        debugOverlay->draw();
    }
    #endif
}

void OverlayRenderer::render() const {
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void OverlayRenderer::shutdown() const {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();
}

void OverlayRenderer::newFrame() const {
    ImGui_ImplSDL2_NewFrame();
    ImGui_ImplOpenGL3_NewFrame();
    ImGui::NewFrame();
}

void OverlayRenderer::setActiveCamera(int cameraId, bool connected) {
    printf("OverlayRenderer::setActiveCamera: %d, connected: %d\n", cameraId, connected);
    std::lock_guard<std::mutex> lock(activeCameraMutex);
    activeCameraOverlay.setActiveCamera(cameraId, connected);
}

void OverlayRenderer::initOperatingInstructions() {
    operatingInstructions = new ImageOverlay("../data/images/OperatingInstructions.png");
    int width = WindowUtils::getInstance().getWindowWidth();
    int height = WindowUtils::getInstance().getWindowHeight();
    operatingInstructions->setCenterPosition(width*0.5f, height*0.5f);
    operatingInstructions->setSize(Constants::OverlayReferenceWidth, Constants::OverlayReferenceHeight);
    operatingInstructions->init();
    operatingInstructionsStartTime = std::chrono::high_resolution_clock::now();
}

#ifdef DEBUG
void OverlayRenderer::setTemperatureMonitoring(TemperatureMonitoring* temperatureMonitoring) {
    this->temperatureMonitoring = temperatureMonitoring;
}
#endif
